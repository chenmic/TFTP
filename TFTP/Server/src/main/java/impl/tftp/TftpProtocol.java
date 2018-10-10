package bgu.spl171.net.impl.tftp;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.concurrent.ConcurrentLinkedQueue;

import bgu.spl171.net.api.bidi.BidiMessagingProtocol;
import bgu.spl171.net.api.bidi.Connections;

public class TftpProtocol implements BidiMessagingProtocol<Command>{
	
	File home;
	ConcurrentLinkedQueue<String> loggedInUserNames;
	ConcurrentLinkedQueue<Integer> loggedInUserIds;
	ConcurrentLinkedQueue<String> uploadingFiles;
	ConcurrentLinkedQueue<String> downloadingFiles;
	private ConcurrentLinkedQueue<Command> dataToSend;
	private Connections<Command> connections;
	private FileOutputStream out;
	private FileInputStream in;
	private String uploadedFile;
	private String downloadedFile;
	private boolean shouldTerminate;
	private int fileLen;
	private String userName;
	private int connectionId;
	private short dataBlockNumber;
	
	public TftpProtocol(File home, ConcurrentLinkedQueue<String> loggedInUserNames, ConcurrentLinkedQueue<Integer> loggedInUserIds, ConcurrentLinkedQueue<String> uploadingFiles, ConcurrentLinkedQueue<String> downloadingFiles){
		this.home = home;
		this.loggedInUserNames = loggedInUserNames;
		this.loggedInUserIds = loggedInUserIds;
		this.uploadingFiles = uploadingFiles;
		this.downloadingFiles = downloadingFiles;
		dataToSend = new ConcurrentLinkedQueue<Command>();
		connections = null;
		out = null;
		in = null;
		uploadedFile = null;
		downloadedFile = null;
		dataBlockNumber = 0;
		fileLen = 0;
		shouldTerminate = false;
		userName = null;
		connectionId = -1;
	}
	
	@Override
	public boolean shouldTerminate() {
		return shouldTerminate;
	}

	@Override
	public void start(int connectionId, Connections<Command> connections) {
		this.connectionId = connectionId;
		this.connections = connections;
	}

	@Override
	public void process(Command message) {
		if (userName==null){
			if (message.opCode==7){
				String name = ((CommandLog)message).getStr();
				synchronized (loggedInUserNames) {
					if (loggedInUserNames.contains(name)){
						connections.send(connectionId, new CommandError((short)7,("User already logged in – Login " + name + " already connected.\0")));
					}else{
						loggedInUserNames.add(name);
						loggedInUserIds.add(connectionId);
						userName = name;
						connections.send(connectionId, new CommandAck((short)0));
					}
				}
					
			}else{
				connections.send(connectionId, new CommandError((short)6,("User not logged in – " + message.opCode + " opcode received before Login completes.\0")));
			}
		}
		else{ /* User is logged in */
			switch (message.opCode){
			case(1): // Read
				downloadedFile = ((CommandRead)message).getStr();
				String readPath = home.getPath() + File.separator + downloadedFile;
				if (Files.exists(Paths.get(readPath))){
					if (!uploadingFiles.contains(downloadedFile)){
						try {
							downloadingFiles.add(downloadedFile);
							byte[] toRead;
							in = new FileInputStream(readPath);
							fileLen = in.available();
							if (fileLen>=512){
								toRead = new byte[512];
								in.read(toRead);
								fileLen-=512;
							}
							else{
								toRead = new byte[fileLen];
								in.read(toRead);
								fileLen=-1;
							}
							connections.send(connectionId, new CommandData((short) toRead.length, ++dataBlockNumber, toRead));
						} catch (IOException e) {
							e.printStackTrace();
						}
					}else{
						downloadedFile = null;
						connections.send(connectionId, new CommandError((short)2,"Access violation – File cannot be read\0"));
					}
				}
				else{
					downloadedFile = null;
					connections.send(connectionId, new CommandError((short)1,"File not found – RRQ of non-existing file\0"));
				}
			break;
			case(2): // Write
				uploadedFile = ((CommandWrite)message).getStr();
				String writePath = home.getPath() + File.separator + uploadedFile;
				if (!Files.exists(Paths.get(writePath))){
					synchronized (uploadingFiles) {
						if(!uploadingFiles.contains(uploadedFile)){
							try {
								
								Files.createFile(Paths.get(writePath));
								uploadingFiles.add(uploadedFile);
								out = new FileOutputStream(writePath);
								connections.send(connectionId, new CommandAck((short)0));
							} catch (IOException e) {
								e.printStackTrace();
							}
						}
					}
				}
				else{
					connections.send(connectionId, new CommandError((short)5,"File already exists – File " + uploadedFile + " exists on server.\0"));
					uploadedFile = null;
				}
				break;
			case(3): // Data // TODO: sync here and in all of the other places that needs it.
				if (home.getUsableSpace()>=((CommandData)message).getSize()){
					if (((CommandData)message).getSize()<512){ //Last data packet
						try {
							out.write(((CommandData)message).getData());
							out.flush();
							out.close();
							out=null;
							connections.send(connectionId, new CommandAck(((CommandData)message).getBlockNumber()));
							sendBroadcast(uploadedFile, (byte) 1);
							uploadingFiles.remove(uploadedFile);
							uploadedFile = null;
						} catch (IOException e) {
							e.printStackTrace();
						}
					}
					else{ //All data packets excluding the last
						try {
							out.write(((CommandData)message).getData());
							out.flush();
							connections.send(connectionId, new CommandAck(((CommandData)message).getBlockNumber()));
						} catch (IOException e) {
							e.printStackTrace();
						}
					}
				}
				else{
					try {
						out.close();
						out=null;
						Files.delete(Paths.get(home.getPath() + File.separator + uploadedFile));
						connections.send(connectionId, new CommandError((short)3,"Disk full – No room in disk.\0"));
						uploadingFiles.remove(uploadedFile);
						uploadedFile = null;
					} catch (IOException e) {
						e.printStackTrace();
					}
				}
				break;
			case(4): // Ack
				short block = ((CommandAck)message).getBlock();
				if (downloadedFile==null){ // Ack a Dir command.
					if (block==dataBlockNumber){
						if (!dataToSend.isEmpty()){
							connections.send(connectionId, dataToSend.poll());
							dataBlockNumber++;
						}else
							dataBlockNumber = 0;
					}
					else{
						dataBlockNumber = 0;
						dataToSend.clear();
						connections.send(connectionId, new CommandError((short)0,"Not defined, aborting last operation.\0"));
					}
				}else{ // Ack a Read command.
					if (block==dataBlockNumber){
						try{
							byte[] toAckRead;
							if (fileLen>=512){ // all packets excluding last.
								toAckRead = new byte[512];
								in.read(toAckRead);
								fileLen-=512;
								connections.send(connectionId, new CommandData((short) toAckRead.length, ++dataBlockNumber, toAckRead));
							}
							else if (fileLen>=0){ // last packet
								toAckRead = new byte[fileLen];
								in.read(toAckRead);
								connections.send(connectionId, new CommandData((short) toAckRead.length, ++dataBlockNumber, toAckRead));
								fileLen=-1;
							}else{
								clearDownload();
								fileLen=0;
							}
						} catch (IOException e){
							e.printStackTrace();
						}
					}
					else{
						clearDownload();
						connections.send(connectionId, new CommandError((short)0,"Not defined, aborting last operation.\0"));
					}
				}
				break;
			case(5): // Error
				if (downloadedFile!=null){
					clearDownload();
				}
				break;
			case(6): // Dir
				String dataStr = "";
				String[] temp = home.list();
				for (String s : temp){
					if (!uploadingFiles.contains(s))
						dataStr = dataStr + s + '\0';
				}
				byte[] dataByte = dataStr.getBytes();
				divideDataBytestoWrite(dataByte);
				dataBlockNumber++;
				connections.send(connectionId, dataToSend.poll());
				break;
			case(7): // Log
				connections.send(connectionId, new CommandError((short)4,"Illegal TFTP operation.\0"));
				break;
			case(8): // Del
				String delName = ((CommandDel)message).getStr();
				String delPath = home.getPath() + File.separator + delName;
				if (Files.exists(Paths.get(delPath))){
					if (!downloadingFiles.contains(delName) && !uploadingFiles.contains(delName)){
						//TODO sync
						try {
							Files.delete(Paths.get(delPath));
							connections.send(connectionId, new CommandAck((short)0));
							sendBroadcast(delName,(byte) 0);
						} catch (IOException e) {
							e.printStackTrace();
						}
					}
					else
						connections.send(connectionId, new CommandError((short)2,"Access violation – File cannot be deleted.\0"));
				}
				else
					connections.send(connectionId, new CommandError((short)1,"File not found – DELRQ of non-existing file\0"));
				break;
			default: // Disc
				loggedInUserNames.remove(userName);
				loggedInUserIds.remove(connectionId);
				shouldTerminate = true;
				userName = null;
				connections.send(connectionId, new CommandAck((short)0));
				break;
			}
		}
	}
	
	private void sendBroadcast(String fileName, byte type){
		// TODO: sync on logged in user ids?
		for (int i : loggedInUserIds){
			connections.send(i, new CommandBcast(type,fileName));
		}
	}
	
	private void divideDataBytestoWrite(byte[] arr){
		ArrayList<byte[]> dataArr = new ArrayList<byte[]>();
		int blocks = arr.length/512;
		int lastBlock = arr.length%512;
		for (int i=0; i<blocks; i++){
			dataArr.add(Arrays.copyOfRange(arr, i*512, ((i+1)*512)));
		}
		dataArr.add(Arrays.copyOfRange(arr, blocks*512, (blocks*512)+lastBlock));
		for (byte[] i : dataArr){
			dataToSend.add(new CommandData((short)i.length, ++dataBlockNumber, i));
		}
		dataBlockNumber=0;
	}
	
	private void clearDownload(){
		try{
			dataBlockNumber = 0;
			downloadingFiles.remove(downloadedFile);
			downloadedFile=null;
			in.close();
			in = null;
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

}

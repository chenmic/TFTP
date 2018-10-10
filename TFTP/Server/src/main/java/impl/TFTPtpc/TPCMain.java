package bgu.spl171.net.impl.TFTPtpc;

import java.io.File;
import java.util.concurrent.ConcurrentLinkedQueue;
import bgu.spl171.net.impl.tftp.PacketEncoderDecoder;
import bgu.spl171.net.impl.tftp.TftpProtocol;
import bgu.spl171.net.srv.Server;

public class TPCMain {

	public static void main(String[] args) {
		File home = new File("Files");
		ConcurrentLinkedQueue<String> loggedInUserNames = new ConcurrentLinkedQueue<String>();
		ConcurrentLinkedQueue<Integer> loggedInUserIds = new ConcurrentLinkedQueue<Integer>();
		ConcurrentLinkedQueue<String> uploadingFiles = new ConcurrentLinkedQueue<String>();
		ConcurrentLinkedQueue<String> downloadingFiles = new ConcurrentLinkedQueue<String>();
		
		Server.threadPerClient(
				Integer.parseInt(args[0]),
                () ->  new TftpProtocol(home, loggedInUserNames, loggedInUserIds, uploadingFiles, downloadingFiles), //protocol factory
                () ->  new PacketEncoderDecoder() //message encoder decoder factory
        ).serve();
	}

	
}

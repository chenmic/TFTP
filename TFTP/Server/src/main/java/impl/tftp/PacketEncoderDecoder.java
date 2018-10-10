package bgu.spl171.net.impl.tftp;


import java.util.Arrays;

import bgu.spl171.net.api.MessageEncoderDecoder;

public class PacketEncoderDecoder implements MessageEncoderDecoder<Command>{

	private byte[] opCodeArr = new byte[2];
	private byte[] content = new byte[1 << 10];
	private int conIndex = 0;
    private int opCodeIndex = 0;
    private short opCode;
    private short dataSize;
    
    
	@Override
	public Command decodeNextByte(byte nextByte) {
		if (opCodeIndex<2){
			opCodeArr[opCodeIndex++] = nextByte;
			if (opCodeIndex == 2){
				opCode = bytesToShort(opCodeArr);
				if (opCode==6){ // Dir
					clear();
					return new CommandDir();
				}
				if (opCode==10){ // Disc
					clear();
					return new CommandDisc();
				}
			}
		}
		else{
			Command ans;
			switch (opCode){
				case(1): // Read
					if (nextByte == '\0'){
						ans = new CommandRead(new String(content,0,conIndex));
						clear();
						return ans;
					}
					pushByte(nextByte);
					break;
				case(2): // Write
					if (nextByte == '\0'){
						ans = new CommandWrite(new String(content,0,conIndex));
						clear();
						return ans;
					}
					pushByte(nextByte);
					break;
				case(3): // Data
					if (conIndex<2){
						pushByte(nextByte);
						if (conIndex == 2)
							dataSize = bytesToShort(Arrays.copyOfRange(content, 0, 2));
					}
					else{
						pushByte(nextByte);
						if (conIndex == dataSize + 4){
							ans = new CommandData(dataSize,bytesToShort(Arrays.copyOfRange(content, 2, 4)),Arrays.copyOfRange(content, 4, conIndex));
							clear();
							return ans;
						}
					}
					break;
				case(4): // Ack
					pushByte(nextByte);
					if (conIndex == 2){
						ans = new CommandAck(bytesToShort(Arrays.copyOfRange(content, 0, 2)));
						clear();
						return ans;
					}
					break;
				case(5): // Error
					if (nextByte == '\0'){
						ans = new CommandError(bytesToShort(Arrays.copyOfRange(content, 0, 2)),new String(Arrays.copyOfRange(content, 2, conIndex)));
						clear();
						return ans;
					}
					pushByte(nextByte);
					break;
				case(7): // Log
					if (nextByte == '\0'){
						ans = new CommandLog(new String(content,0,conIndex));
						clear();
						return ans;
					}
					pushByte(nextByte);
					break;
				case(8): // Del
					if (nextByte == '\0'){
						ans = new CommandDel(new String(content,0,conIndex));
						clear();
						return ans;
					}
					pushByte(nextByte);
					break;
				case(9): // Bcast
					if (nextByte == '\0'){
						ans = new CommandBcast(content[0],new String(content,1,conIndex-1));
						clear();
						return ans;
					}
					pushByte(nextByte);
					break;
				default:
					break;
					
			}
		}
		return null;
	}
	
	private void pushByte(byte nextByte) {
        if (conIndex >= content.length)
        	content = Arrays.copyOf(content, conIndex * 2);
        content[conIndex++] = nextByte;
    }
	
	private void clear(){
		conIndex = 0;
		opCodeIndex = 0;
	}

	@Override
	public byte[] encode(Command message) {
		byte[] temp;
		byte[] opArr = shortToBytes(message.opCode);
		switch(message.opCode){
			case(1): // Read
				temp = (((CommandRead) message).getStr() + '\0').getBytes();
				break;
			case(2): // Write
				temp = (((CommandWrite) message).getStr() + '\0').getBytes();
				break;
			case(3): // Data
				byte[] size = shortToBytes(((CommandData) message).getSize());
				byte[] blockNum = shortToBytes(((CommandData) message).getBlockNumber());
				byte[] data = ((CommandData) message).getData();
				temp = new byte[data.length + size.length + blockNum.length];
				System.arraycopy(size, 0, temp, 0, 2);
				System.arraycopy(blockNum, 0, temp, 2, 2);
				System.arraycopy(data, 0, temp, 4, data.length);
				break;
			case(4): // Ack
				temp = shortToBytes(((CommandAck) message).getBlock());
				break;
			case(5): // Error
				byte[] msgStr = ((CommandError) message).getErrorMsg().getBytes();
				byte[] errCode = shortToBytes(((CommandError) message).getErrorCode());
				temp = new byte[msgStr.length + errCode.length];
				System.arraycopy(errCode, 0, temp, 0, errCode.length);
				System.arraycopy(msgStr, 0, temp, 2, msgStr.length);
				break;
			case(6): // Dir
				temp = new byte[0];
				break;
			case(7): // Log
				temp = (((CommandLog) message).getStr() + '\0').getBytes();		
				break;
			case(8): // Del
				temp = (((CommandDel) message).getStr() + '\0').getBytes();
				break;
			case(9): // Bcast
				byte[] name = (((CommandBcast) message).getName() + '\0').getBytes();
				temp = new byte[name.length + 1];
				temp[0] = ((CommandBcast) message).getType();
				System.arraycopy(name, 0, temp, 1, name.length);
				break;
			default: // Disc
				temp = new byte[0];
				break;
		}
		byte[] ans = new byte[opArr.length + temp.length];
		System.arraycopy(opArr, 0, ans, 0, opArr.length);
		System.arraycopy(temp, 0, ans, 2, temp.length);
		return ans;
	}
	
	private short bytesToShort(byte[] byteArr){
	    short result = (short)((byteArr[0] & 0xff) << 8);
	    result += (short)(byteArr[1] & 0xff);
	    return result;
	}
	
	private byte[] shortToBytes(short num)
	{
	    byte[] bytesArr = new byte[2];
	    bytesArr[0] = (byte)((num >> 8) & 0xFF);
	    bytesArr[1] = (byte)(num & 0xFF);
	    return bytesArr;
	}

}

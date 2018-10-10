package bgu.spl171.net.impl.tftp;

public class CommandBcast extends Command {

	private String fileName;
	private byte type;
	
	public CommandBcast(byte type, String fileName) {
		super((short) 9);
		this.type = type;
		this.fileName = fileName;
	}
	
	public String getName(){
		return fileName;
	}
	
	public byte getType(){
		return type;
	}

	
}

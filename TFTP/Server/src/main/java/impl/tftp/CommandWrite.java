package bgu.spl171.net.impl.tftp;

public class CommandWrite extends Command {

	protected String fileName;
	
	public CommandWrite(String fileName) {
		super((short) 2);
		this.fileName = fileName;
	}

	public String getStr(){
		return fileName;
	}
	

}

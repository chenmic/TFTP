package bgu.spl171.net.impl.tftp;

public class CommandRead extends Command {
	
	protected String fileName;
	
	protected CommandRead(String fileName) {
		super((short) 1);
		this.fileName = fileName;
	}

	public String getStr(){
		return fileName;
	}
	

}

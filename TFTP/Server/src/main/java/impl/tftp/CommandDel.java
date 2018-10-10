package bgu.spl171.net.impl.tftp;

public class CommandDel extends Command {

	private String fileName;
	
	public CommandDel(String fileName) {
		super((short) 8);
		this.fileName = fileName;
	}
	
	public String getStr(){
		return fileName;
	}


}

package bgu.spl171.net.impl.tftp;

public class CommandLog extends Command {

	private String userName;
	
	public CommandLog(String userName) {
		super((short) 7);
		this.userName = userName;
	}

	public String getStr(){
		return userName;
	}
	

}

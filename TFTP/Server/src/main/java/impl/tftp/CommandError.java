package bgu.spl171.net.impl.tftp;

public class CommandError extends Command {

	private short errorCode;
	private String errorMsg;
	
	public CommandError(short errorCode, String errorMsg) {
		super((short) 5);
		this.errorCode = errorCode;
		this.errorMsg = errorMsg;
	}
	
	public short getErrorCode(){
		return errorCode;
	}
	
	public String getErrorMsg(){
		return errorMsg;
	}

}

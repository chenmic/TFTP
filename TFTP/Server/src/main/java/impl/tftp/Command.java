package bgu.spl171.net.impl.tftp;

public abstract class Command {
	
	protected short opCode;
	
	protected Command(short opCode){
		this.opCode = opCode;
	}

	public short getOpCode() {
		return opCode;
	}

}

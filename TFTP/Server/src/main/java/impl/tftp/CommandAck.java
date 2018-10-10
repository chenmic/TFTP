package bgu.spl171.net.impl.tftp;

public class CommandAck extends Command {

	private short blockNumber;
	
	public CommandAck(short blockNumber) {
		super((short) 4);
		this.blockNumber = blockNumber;
	}
	
	public short getBlock(){
		return blockNumber;
	}

	
}

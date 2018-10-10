package bgu.spl171.net.impl.tftp;

public class CommandData extends Command {
	
	private short packetSize;
	private short blockNumber;
	byte[] data;
	
	public CommandData(short packetSize, short blockNumber, byte[] data) {
		super((short) 3);
		this.packetSize = packetSize;
		this.blockNumber = blockNumber;
		this.data = data;
	}

	public short getSize() {
		return packetSize;
	}

	public short getBlockNumber() {
		return blockNumber;
	}

	public byte[] getData() {
		return data;
	}
	

}

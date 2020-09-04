enum SocketAddressFamily
{
	INET = AF_INET,
	INET6 = AF_INET6
};
class SocketUtil
{
	public:
	static bool StaticInit();
	static void CleanUp();
	static void ReportError(const char* inOperationDesc);
	static int GetLastError();

	static UDPSocketPtr CreateUDPSocket(SocketAddressFamily inFamily);
	static TCPSocketPtr CreateTCPSocket(SocketAddressFamily inFamily);
	private:
	

};


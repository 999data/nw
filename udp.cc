#include <fstream>
#include <ns3/core-module.h>
#include <ns3/csma-module.h>
#include <ns3/applications-module.h>
#include <ns3/internet-module.h>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("UdpClientServerExample");

int 
main (int argc, char *argv[])
{

	LogComponentEnable ("UdpClient", LOG_LEVEL_INFO);
	LogComponentEnable ("UdpServer", LOG_LEVEL_INFO);
	
	bool useV6 = false;
	Address serverAddress;
	
	CommandLine cmd (__FILE__);
	cmd.AddValue ("useV6", "use Ipv6", useV6);
	cmd.Parse (argc, argv);
	
	NS_LOG_INFO ("Create Nodes.");
	NodeContainer n;
	n.Create (2);
	
	InternetStackHelper internet;
	internet.Install (n);
	
	NS_LOG_INFO ("Create Channels.");
	
	CsmaHelper csma;
	csma.SetChannelAttribute ("DataRate", DataRateValue (DataRate (5000000)));
	csma.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (2)));
	csma.SetDeviceAttribute ("Mtu", UintegerValue (1400));
	NetDeviceContainer d = csma.Install (n);
	
	NS_LOG_INFO ("Assign IP Addresses");
	if (useV6 == false)
	{
		Ipv4AddressHelper ipv4;
		ipv4.SetBase ("10.1.1.0", "255.255.255.0");
		Ipv4InterfaceContainer i = ipv4.Assign (d);
		serverAddress = Address (i.GetAddress (1));
	}
	else
	{
		Ipv6AddressHelper ipv6;
		ipv6.SetBase ("2001:0000:f00d:cafe::", Ipv6Prefix (64));
		Ipv6InterfaceContainer i6 = ipv6.Assign (d);
		serverAddress = Address(i6.GetAddress (1,1));
	}
	
	NS_LOG_INFO ("Create Applications.");
	
	uint16_t port = 4000;
	UdpServerHelper server (port);
	ApplicationContainer apps = server.Install (n.Get (1));
	apps.Start (Seconds (1.0));
	apps.Stop (Seconds (10.0));
	
	uint32_t MaxPacketSize = 1024;
	Time interPacketInterval = Seconds (0.05);
	uint32_t maxPacketCount = 320;
	UdpClientHelper client (serverAddress, port);
	client.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
	client.SetAttribute ("Interval", TimeValue (interPacketInterval));
	client.SetAttribute ("PacketSize", UintegerValue (MaxPacketSize));
	apps = client.Install (n.Get (0));
	apps.Start (Seconds (2.0));
	apps.Stop (Seconds (10.0));
	
	NS_LOG_INFO ("Run Simulation.");
	Simulator::Run ();
	Simulator::Destroy();
	NS_LOG_INFO ("Done.");
}

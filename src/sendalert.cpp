/*  From Bitcoincode
So you need to broadcast an alert...
... here's what to do:

1. Copy sendalert.cpp into your bitcoind build directory

2. Decrypt the alert keys
  copy the decrypted file as alertkeys.h into the src/ directory.

3. Modify the alert parameters in sendalert.cpp
  See the comments in the code for what does what.

4. Add sendalert.cpp to the src/Makefile.am so it gets built:

    libbitcoin_server_a_SOURCES = \
      sendalert.cpp \
      ... etc

5. Update init.cpp to launch the send alert thread. 
  Define the thread function as external at the top of init.cpp:

    extern void ThreadSendAlert();

  Add this call at the end of AppInit2:

    threadGroup.create_thread(boost::bind(ThreadSendAlert));

6. build bitcoind, then run it with -printalert or -sendalert
  I usually run it like this:
   ./bitcoind -printtoconsole -sendalert

One minute after starting up the alert will be broadcast. It is then
flooded through the network until the nRelayUntil time, and will be
active until nExpiration OR the alert is cancelled.

If you screw up something, send another alert with nCancel set to cancel
the bad alert.
*/
#include "main.h"
#include "net.h"
#include "alert.h"
#include "init.h"

static const int64_t DAYS = 24 * 60 * 60;

//void ThreadSendAlert()

Value sendalert(const Array& params, bool fHelp)


{
  //  MilliSleep(60*1000); // Wait a minute so we get connected
    if (!mapArgs.count("-sendalert") && !mapArgs.count("-printalert"))
        return "no -sendalert or -printalert paramter provided";
    
    // Modification for Feathercoin. we send an alert as command, not with a special compiled 
    // version of the client
    if (fHelp || params.size() < 6)
        throw runtime_error(
            "sendalert <message> <privatekey> <minver> <maxver> <priority> <id> [cancelupto]\n"
            "<message> is the alert text message\n"
            "<privatekey> is base58 hex string of alert master private key\n"
            "<minver> is the minimum applicable internal client version\n"
            "<maxver> is the maximum applicable internal client version\n"
            "<priority> is integer priority number\n"
            "<id> is the alert id\n"
            "[cancelupto] cancels all alert id's up to this number\n"
            "Returns true or false.");

	// Prepare the alert message
	
	CAlert alert;
	
	alert.strStatusBar = params[0].get_str();
	alert.nMinVer = params[2].get_int();
	alert.nMaxVer = params[3].get_int();
	alert.nPriority = params[4].get_int();
	alert.nID = params[5].get_int();
	if (params.size() > 6)
	    alert.nCancel = params[6].get_int();
	else
	  alert.nCancel       = 0;   // cancels previous messages up to this ID number
	alert.nVersion = PROTOCOL_VERSION;  // not used anymore in 11.2
	alert.nRelayUntil = GetAdjustedTime() + 365*24*60*60;
	alert.nExpiration = GetAdjustedTime() + 365*24*60*60;
  
    //
    // Alerts are relayed around the network until nRelayUntil, flood
    // filling to every node.
    // After the relay time is past, new nodes are told about alerts
    // when they connect to peers, until either nExpiration or
    // the alert is cancelled by a newer alert.
    // Nodes never save alerts to disk, they are in-memory-only.
    //
    /* original code from bitcoin
     */
  //  CAlert alert;
  //  alert.nRelayUntil   = GetTime() + 15 * 60;
  //  alert.nExpiration   = GetTime() + 365 * 60 * 60;
  //  alert.nID           = 1040;  // use https://en.bitcoin.it/wiki/Alerts to keep track of alert IDs
  //  alert.nCancel       = 0;   // cancels previous messages up to this ID number

    // These versions are protocol versions
    // 60002 : 0.7.*
    // 70001 : 0.8.*
    // 70002 : 0.9.*
   // alert.nMinVer       = 70002;
   // alert.nMaxVer       = 70010;

    
    //
    // main.cpp: 
    //  1000 for Misc warnings like out of disk space and clock is wrong
    //  2000 for longer invalid proof-of-work chain 
    //  Higher numbers mean higher priority
    // alert.nPriority     = 5000;
    // alert.strComment    = "testcomment";
    // alert.strStatusBar  = "URGENT: Upgrade required: see https://www.bitcoin.org/heartbleed";

    // Set specific client version/versions here. If setSubVer is empty, no filtering on subver is done:
    // alert.setSubVer.insert(std::string("/Satoshi:0.7.2/"));

    // Sign
    //#include "alertkeys.h"

   // std::vector<unsigned char> vchTmp(ParseHex(TestNet() ? pszTestNetPrivKey : pszPrivKey));
    std::vector<unsigned char> vchTmp(ParseHex(params[1].get_str()));
    printf("%s\n", params[1].get_str().c_str());
    CPrivKey vchPrivKey(vchTmp.begin(), vchTmp.end());
    printf("2\n");
    CDataStream sMsg(SER_NETWORK, CLIENT_VERSION);
    printf("3\n");
    sMsg << *(CUnsignedAlert*)&alert;
    printf("4\n");
    alert.vchMsg = std::vector<unsigned char>(sMsg.begin(), sMsg.end());
    printf("ThreadSendAlert() : key.SetPrivKey");
    CKey key;
    if (!key.SetPrivKey(vchPrivKey, false))
    {
       printf("ThreadSendAlert() : key.SetPrivKey failed\n");
 //	return;
      return ("ThreadSendAlert() : key.SetPrivKey failed\n");
      }
    if (!key.Sign(Hash(alert.vchMsg.begin(), alert.vchMsg.end()), alert.vchSig))
    {
        printf("ThreadSendAlert() : key.Sign failed\n");
        return("ThreadSendAlert() : key.Sign failed\n");
        
    }

    // Test
    CDataStream sBuffer(SER_NETWORK, CLIENT_VERSION);
    sBuffer << alert;
    CAlert alert2;
    sBuffer >> alert2;
//    if (!alert2.CheckSignature())
//    {
//        printf("ThreadSendAlert() : CheckSignature failed\n");
//        ;
//    }
    assert(alert2.vchMsg == alert.vchMsg);
    assert(alert2.vchSig == alert.vchSig);
    alert.SetNull();
    printf("\nThreadSendAlert:\n");
    printf("hash=%s\n", alert2.GetHash().ToString().c_str());
//    alert2.print();
    printf("vchMsg=%s\n", HexStr(alert2.vchMsg).c_str());
    printf("vchSig=%s\n", HexStr(alert2.vchSig).c_str());

    // Confirm
    if (!mapArgs.count("-sendalert"))
        printf ("ThreadSendAlert(): no -sendalert parameter");
        return ("ThreadSendAlert(): no -sendalert parameter");
    while (vNodes.size() < 1 && !ShutdownRequested())
        MilliSleep(500);
    if (ShutdownRequested())
        return "ThreadSendAlert(): shutdown requested";
#ifdef QT_GUI
    if (ThreadSafeMessageBox("Send alert?", "ThreadSendAlert", wxYES_NO | wxNO_DEFAULT) != wxYES)
        return "sendalert xxx ";
    if (ThreadSafeMessageBox("Send alert, are you sure?", "ThreadSendAlert", wxYES_NO | wxNO_DEFAULT) != wxYES)
    {
        ThreadSafeMessageBox("Nothing sent", "ThreadSendAlert", wxOK);
        return "sendalert :Nothing sent ";
    }
#endif

    // Send
    printf("ThreadSendAlert() : Sending alert\n");
    int nSent = 0;
    {
        LOCK(cs_vNodes);
        BOOST_FOREACH(CNode* pnode, vNodes)
        {
            if (alert2.RelayTo(pnode))
            {
                printf("ThreadSendAlert() : Sent alert to %s\n", pnode->addr.ToString().c_str());
                nSent++;
            }
        }
    }
    printf("ThreadSendAlert() : Alert sent to %d nodes\n", nSent);
    return (printf("ThreadSendAlert() : Alert sent to %d nodes\n", nSent));
}

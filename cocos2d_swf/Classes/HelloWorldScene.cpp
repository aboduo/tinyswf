#include "HelloWorldScene.h"
#include "network/HttpRequest.h"
#include <string>
//#include <vld.h>

#include "CCFlash.h"

#ifdef WIN32
#include <windows.h>
#include <Wincrypt.h>
#include <Iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")

bool getUUID(std::string& uuid) {
	// Get the buffer length required for IP_ADAPTER_INFO.
	ULONG BufferLength = 0;
	BYTE* pBuffer = 0;
	if (ERROR_BUFFER_OVERFLOW == GetAdaptersInfo( 0, &BufferLength )) {
		// Now the BufferLength contain the required buffer length.
		// Allocate necessary buffer.
		pBuffer = new BYTE[ BufferLength ];
	} else {
		// Error occurred. handle it accordingly.
		return false;
	}

	// Get the Adapter Information.
	PIP_ADAPTER_INFO pAdapterInfo = reinterpret_cast<PIP_ADAPTER_INFO>(pBuffer);
	GetAdaptersInfo( pAdapterInfo, &BufferLength );
/*
	// Iterate the network adapters and print their MAC address.
	while( pAdapterInfo ) {
		// Assuming pAdapterInfo->AddressLength is 6.
		printf ("%02x:%02x:%02x:%02x:%02x:%02x \n",
			pAdapterInfo->Address[0],pAdapterInfo->Address[1],pAdapterInfo->Address[2],
			pAdapterInfo->Address[3],pAdapterInfo->Address[4],pAdapterInfo->Address[5]);
		// Get next adapter info.
		pAdapterInfo = pAdapterInfo->Next;
	}
*/
	// start encrypt
	HCRYPTPROV hProv = 0;
    HCRYPTHASH hHash = 0;
	CryptAcquireContext(&hProv, NULL,  NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);
	CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash);
	CryptHashData(hHash, pAdapterInfo->Address, 6, 0);

	const int MD5LEN = 16;
	const CHAR rgbDigits[] = "0123456789abcdef";
	DWORD cbHash = MD5LEN;
	BYTE rgbHash[MD5LEN];
	uuid.resize(MD5LEN*2);
    if (CryptGetHashParam(hHash, HP_HASHVAL, rgbHash, &cbHash, 0)) {
        for (DWORD i = 0; i < cbHash; i++) {
			uuid[2*i] = rgbDigits[rgbHash[i] >> 4];
			uuid[2*i+1] = rgbDigits[rgbHash[i] & 0xf];
        }
    }
    CryptDestroyHash(hHash);
    CryptReleaseContext(hProv, 0);

	// deallocate the buffer.
	delete[] pBuffer;
	return true;
}
#endif//WIN32

USING_NS_CC;
USING_NS_CC_EXT;

CCScene* HelloWorld::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    HelloWorld *layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

HelloWorld::~HelloWorld() {
    delete tinyswf::Renderer::getRenderer();
	tinyswf::SWF::terminate();
    CCHttpClient::getInstance()->destroyInstance();
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !CCLayer::init() )
    {
        return false;
    }
    
    CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
    CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();

	/////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    CCMenuItemImage *pCloseItem = CCMenuItemImage::create(
                                        "CloseNormal.png",
                                        "CloseSelected.png",
                                        this,
                                        menu_selector(HelloWorld::menuCloseCallback));
    
	pCloseItem->setPosition(ccp(origin.x + visibleSize.width - pCloseItem->getContentSize().width/2 ,
                                origin.y + pCloseItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    CCMenu* pMenu = CCMenu::create(pCloseItem, NULL);
    pMenu->setPosition(CCPointZero);
    this->addChild(pMenu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
    
    CCLabelTTF* pLabel = CCLabelTTF::create("Hello World", "Arial", 24);
    
    // position the label on the center of the screen
    pLabel->setPosition(ccp(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height - pLabel->getContentSize().height));

    // add the label as a child to this layer
    this->addChild(pLabel, 1);

    // add "HelloWorld" splash screen"
    CCSprite* pSprite = CCSprite::create("HelloWorld.png");

    // position the sprite on the center of the screen
    pSprite->setPosition(ccp(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

    // add the sprite as a child to this layer
    this->addChild(pSprite, 0);

	// initialize flash
    tinyswf::SWF::initialize(myLoadAssetCallback, 256*1024); // 256kB buffer for libtess2
    tinyswf::Renderer::setRenderer(new CCFlashRenderer);
    CCFlash* pFlash = CCFlash::create("test2.swf");
	pFlash->scheduleUpdate();
	this->addChild(pFlash,1);
	
#if 0
	CCHttpRequest* request = new CCHttpRequest();
	request->setUrl("https://localhost/codeigniter/user/login");
	request->setRequestType(CCHttpRequest::kHttpPost);
	request->setResponseCallback(this, httpresponse_selector(HelloWorld::onHttpRequestCompleted));
	request->setTag("POST login");

	std::string uuid;
	getUUID(uuid);
	std::string post = "uuid="+uuid;
	request->setRequestData(post.c_str(), post.length()); 

	CCHttpClient::getInstance()->send(request);
	request->release();
#endif
    return true;
}

void HelloWorld::menuCloseCallback(CCObject* pSender)
{
    CCDirector::sharedDirector()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}


void HelloWorld::onHttpRequestCompleted(CCHttpClient *sender, CCHttpResponse *response)
{
    if (!response)
    {
        return;
    }
    
    // You can get original request type from: response->request->reqType
    if (0 != strlen(response->getHttpRequest()->getTag())) 
    {
        CCLog("%s completed", response->getHttpRequest()->getTag());
    }
    
    int statusCode = response->getResponseCode();
    char statusString[64] = {};
    sprintf(statusString, "HTTP Status Code: %d, tag = %s", statusCode, response->getHttpRequest()->getTag());
    CCLog("response code: %d", statusCode);
    
    if (!response->isSucceed()) 
    {
        CCLog("response failed");
        CCLog("error buffer: %s", response->getErrorBuffer());
        return;
    }
    
    // dump data
    CCLog("Http Test, dump data: %s\n", response->getResponseData()->c_str());
}


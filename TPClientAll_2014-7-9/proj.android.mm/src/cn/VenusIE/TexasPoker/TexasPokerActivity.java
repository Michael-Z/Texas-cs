package cn.VenusIE.TexasPoker;

import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.HashMap;
import java.util.Locale;

import mm.purchasesdk.Purchase;

import org.cocos2dx.lib.Cocos2dxActivity;

import com.umeng.analytics.MobclickAgent;
import com.umeng.fb.FeedbackAgent;
import com.umeng.update.UmengUpdateAgent;

import android.R.integer;
import android.bluetooth.BluetoothAdapter;
import android.content.Context;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.net.wifi.WifiManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.provider.Settings.Secure;
import android.telephony.TelephonyManager;
import android.util.Log;
import net.youmi.android.AdManager;
import net.youmi.android.offers.OffersManager;
import net.youmi.android.offers.OffersWallDialogListener;
import android.content.Intent;
import android.content.ComponentName;
import android.content.*;
import android.net.*;
import android.database.*;

public class TexasPokerActivity extends Cocos2dxActivity
{	
	private static String DeviceID = "";
	public static Context mContext;
	public static TexasPokerActivity mActivity;
	public static Purchase purchase;
	private static final String APPID = "300002914196";
	private static final String APPKEY = "C56472F7AD429FB2";
	private static IAPListener mListener;
	public static final int PURCHASE_MSG = 0;
	public static final int SHOW_WALL_MSG = 1;
	public static int VersionCode = 1000;
	
	public static void sendFeeInfo(String qudao,String orderid,String paycode,String orderinfo)
	{
		OnPurchaseFeed(qudao, orderid,paycode, orderinfo);
	}
	
	private native static void OnPurchaseFeed(String qudao, String orderid, String paycode, String orderinfo);
	
	private native static void OnWallClose();
	
    public static Handler mHandler= new Handler(){
		public void handleMessage(Message msg) {
			if(msg.arg1==PURCHASE_MSG)
			{
				String itemPayCode = "";
				switch(msg.arg2)
				{
				case 0:
					itemPayCode = "30000291419601";
					break;
				case 1:
					itemPayCode = "30000291419602";
					break;
				case 2:
					itemPayCode = "30000291419603";
					break;
				case 3:
					itemPayCode = "30000291419604";
					break;
				case 4:
					itemPayCode = "30000291419605";
					break;
				}
				if(itemPayCode.equals("") != true)
				{
					purchase.order(mContext, itemPayCode,1,DeviceID ,mListener);
				}
			}
			else
			{
				OffersManager.getInstance(mActivity).showOffersWallDialog(mActivity, new OffersWallDialogListener()
				{
					@Override
					public void onDialogClose()
					{
						OnWallClose();
					}
				});
			}
		}
	};
	
	public static int jni_GetVersionCode()
	{
		return VersionCode;
	}

	public static void Purchase(int itemid)
	{
		Message msg = new Message();
		msg.arg1 = PURCHASE_MSG;
		msg.arg2 = itemid;
		mHandler.sendMessage(msg);
	}
	
	public static void jni_ShowOffersWall()
	{
		Message msg = new Message();
		msg.arg1 = SHOW_WALL_MSG;
		mHandler.sendMessage(msg);
	}
	
	private boolean hasShortcut()
	{
		boolean isInstallShortcut = false;
		final ContentResolver cr = getContentResolver();
		final String AUTHORITY ="com.android.launcher.settings";
		final Uri CONTENT_URI = Uri.parse("content://" +AUTHORITY + "/favorites?notify=true");
		Cursor c = cr.query(CONTENT_URI,new String[] {"title","iconResource" },"title=?",
			new String[] {getString(R.string.app_name).trim()}, null);
		if(c!=null && c.getCount()>0)
		{
			isInstallShortcut = true ;
	    }
		return isInstallShortcut ;
	}
	
	private void addShortcut()
	{
	    Intent shortcut = new Intent("com.android.launcher.action.INSTALL_SHORTCUT");
	    shortcut.putExtra(Intent.EXTRA_SHORTCUT_NAME, getString(R.string.app_name)); 
	    shortcut.putExtra("duplicate", false);
	    ComponentName comp = new ComponentName(this.getPackageName(), "."+this.getLocalClassName());
	    shortcut.putExtra(Intent.EXTRA_SHORTCUT_INTENT, new Intent(Intent.ACTION_MAIN).setComponent(comp));
	    shortcut.putExtra(Intent.EXTRA_SHORTCUT_ICON_RESOURCE, Intent.ShortcutIconResource.fromContext(this, R.drawable.ic_launcher));
	    sendBroadcast(shortcut);
	}  

	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		if(!hasShortcut())
		{
			addShortcut();
		}
		try
		{
			VersionCode = getPackageManager().getPackageInfo(getPackageName(), 0).versionCode;
		}
		catch(Exception e)
		{
			e.printStackTrace();
		}
		DeviceID = GetDeviceID();
		Log.d("DeviceID", DeviceID);
		UmengUpdateAgent.update(this);
		mContext = this;
		mActivity = this;
		IAPHandler iapHandler = new IAPHandler(this);
		mListener = new IAPListener(this, iapHandler);
		purchase = Purchase.getInstance();
		purchase.setAppInfo(APPID, APPKEY);
		purchase.init(this, mListener);
		AdManager.getInstance(this).init("fb249e0044ff63c1","194c08d156ffaf4e", false);
		OffersManager.getInstance(this).setCustomUserId(DeviceID);
		OffersManager.getInstance(this).onAppLaunch();
	}
	
	protected void onDestroy()
	{
		OffersManager.getInstance(this).onAppExit();
		super.onDestroy();
	}

	static
	{
		System.loadLibrary("TexasPoker");
	}

	public void onResume() {
	    super.onResume();
	    MobclickAgent.onResume(this);
	}
	
	public void onPause() {
	    super.onPause();
	    MobclickAgent.onPause(this);
	}
	
	public static String jni_GetDeviceName()
	{
		return Build.MODEL;
	}
	
	public static String jni_GetLanguage()
	{
		String kLanguage = Locale.getDefault().getLanguage();
		kLanguage += "_";
		kLanguage += Locale.getDefault().getCountry();
		return kLanguage;
	}

	public static String jni_GetDeviceID()
	{
		Log.d("DeviceID", DeviceID);
		return DeviceID;
	}
	
	public static void jni_OnEvent(String strEvent)
	{
		MobclickAgent.onEvent(mContext, strEvent);
	}
	
	public static void jni_OnEventValues(String strEvent, String[] astrValue)
	{
		int iLen = astrValue.length;
		HashMap<String, String> kValue = new HashMap<String, String>();
		for(int i = 0; i < (iLen >> 1); ++i)
		{
			kValue.put(astrValue[i<<1], astrValue[(i<<1)+1]);
		}
		MobclickAgent.onEvent(mContext, strEvent, kValue);
	}
	
	public static void jni_OnEventBegin(String strEvent)
	{
		MobclickAgent.onEventBegin(mContext, strEvent);
	}
	
	public static void jni_OnEventEnd(String strEvent)
	{
		MobclickAgent.onEventEnd(mContext, strEvent);
	}
	
	public static void jni_OnPageStart(String strEvent)
	{
		MobclickAgent.onPageStart(strEvent);
	}
	
	public static void jni_OnPageEnd(String strEvent)
	{
		MobclickAgent.onPageEnd(strEvent);
	}
	
	public static void ShowFeedBack()
	{
		FeedbackAgent agent = new FeedbackAgent(TexasPokerActivity.getContext());
	    agent.startFeedbackActivity();
	}

	private String GetDeviceID()
	{
		String szImei = "";
		try
		{
			TelephonyManager TelephonyMgr = (TelephonyManager) getSystemService(TELEPHONY_SERVICE);
			szImei = TelephonyMgr.getDeviceId();
		}
		catch (SecurityException e)
		{
			e.printStackTrace();
		}
		finally
		{
			if (szImei == null || "".equals(szImei))
			{
				szImei = "";
			}
		}
		String m_szDevIDShort = "35" + Build.BOARD.length() % 10 + Build.BRAND.length() % 10 + Build.CPU_ABI.length() % 10 + Build.DEVICE.length() % 10 + Build.DISPLAY.length() % 10 + Build.HOST.length() % 10 + Build.ID.length() % 10 + Build.MANUFACTURER.length() % 10 + Build.MODEL.length() % 10 + Build.PRODUCT.length() % 10 + Build.TAGS.length() % 10 + Build.TYPE.length() % 10 + Build.USER.length() % 10;
		String m_szAndroidID = Secure.getString(getContentResolver(), Secure.ANDROID_ID);
		String m_szWLANMAC = "";
		try
		{
			WifiManager wm = (WifiManager) getSystemService(Context.WIFI_SERVICE);
			m_szWLANMAC = wm.getConnectionInfo().getMacAddress();
		}
		catch (SecurityException e)
		{
			e.printStackTrace();
		}
		finally
		{
			if (m_szWLANMAC == null || "".equals(m_szWLANMAC))
			{
				m_szWLANMAC = "";
			}
		}
		String m_szBTMAC = "";
		try
		{
			BluetoothAdapter m_BluetoothAdapter = null;
			m_BluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
			m_szBTMAC = m_BluetoothAdapter.getAddress();
		}
		catch (SecurityException e)
		{
			e.printStackTrace();
		}
		finally
		{
			if (m_szBTMAC == null || "".equals(m_szBTMAC))
			{
				m_szBTMAC = "";
			}
		}
		String m_szLongID = szImei + m_szDevIDShort + m_szAndroidID + m_szWLANMAC + m_szBTMAC;

		MessageDigest m = null;
		try
		{
			m = MessageDigest.getInstance("MD5");
		}
		catch (NoSuchAlgorithmException e)
		{
			e.printStackTrace();
		}
		m.update(m_szLongID.getBytes(), 0, m_szLongID.length());
		byte p_md5Data[] = m.digest();
		String m_szUniqueID = new String();
		for (int i = 0; i < p_md5Data.length; ++i)
		{
			int b = (0xFF & p_md5Data[i]);
			if (b <= 0xF) m_szUniqueID += "0";

			m_szUniqueID += Integer.toHexString(b);
		}
		m_szUniqueID = m_szUniqueID.toUpperCase();
		return m_szUniqueID;
	}
}

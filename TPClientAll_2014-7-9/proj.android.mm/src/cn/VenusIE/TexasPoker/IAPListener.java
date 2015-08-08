package cn.VenusIE.TexasPoker;

import java.util.HashMap;

import mm.purchasesdk.OnPurchaseListener;
import mm.purchasesdk.Purchase;
import mm.purchasesdk.PurchaseCode;
import android.app.Activity;
import android.content.Context;
import android.os.Message;
import android.util.Log;

public class IAPListener implements OnPurchaseListener {
	private final String TAG = "IAPListener";
	private Activity context;
	private IAPHandler iapHandler;

	public IAPListener(Context context, IAPHandler iapHandler) {
		this.context = (Activity)context;
		this.iapHandler = iapHandler;
	}

	@Override
	public void onAfterApply() {

	}

	@Override
	public void onAfterDownload() {

	}

	@Override
	public void onBeforeApply() {

	}

	@Override
	public void onBeforeDownload() {

	}

	@Override
	public void onInitFinish(int code) {
		//Log.d(TAG, "onInitFinish, status code = " + code);
	}

	@Override
	public void onBillingFinish(int code, HashMap arg1) {
		//Log.d(TAG, "onBillingFinish, status code = " + code);
		String result = "";
		Message message = iapHandler.obtainMessage(IAPHandler.BILL_FINISH);
		// 姝ゆ璁㈣喘鐨刼rderID
		String orderID = null;
		// 鍟嗗搧鐨刾aycode
		String paycode = null;
		// 鍟嗗搧鐨勬湁鏁堟湡(浠呯璧佺被鍨嬪晢鍝佹湁鏁�)
		String leftday = null;
		// 鍟嗗搧鐨勪氦鏄� ID锛岀敤鎴峰彲浠ユ牴鎹繖涓氦鏄揑D锛屾煡璇㈠晢鍝佹槸鍚﹀凡缁忎氦鏄�
		String tradeID = null;
		
		String ordertype = null;
		if (code == PurchaseCode.ORDER_OK || (code == PurchaseCode.AUTH_OK)) {
			/**
			 * 鍟嗗搧璐拱鎴愬姛鎴栬�呭凡缁忚喘涔般�� 姝ゆ椂浼氳繑鍥炲晢鍝佺殑paycode锛宱rderID,浠ュ強鍓╀綑鏃堕棿(绉熻祦绫诲瀷鍟嗗搧)
			 */
			if (arg1 != null) {
				orderID = (String) arg1.get(OnPurchaseListener.ORDERID);
				if (orderID != null && orderID.trim().length() != 0) {
					result = result + "orderID=" + orderID;
				}
				paycode = (String) arg1.get(OnPurchaseListener.PAYCODE);
				if (paycode != null && paycode.trim().length() != 0) {
					result = result + ",paycode=" + paycode;
				}
				tradeID = (String) arg1.get(OnPurchaseListener.TRADEID);
				if (tradeID != null && tradeID.trim().length() != 0) {
					result = result + ",tradeID=" + tradeID;
				}
				leftday = (String) arg1.get(OnPurchaseListener.LEFTDAY);
				if (leftday != null && leftday.trim().length() != 0) {
					result = result + ",leftday=" + leftday;
				}

				TexasPokerActivity.sendFeeInfo("MM",orderID,paycode,result);
			}
		} else {
			/**
			 * 琛ㄧず璁㈣喘澶辫触銆�
			 */
			result = "billing failed:" + Purchase.getReason(code);
		}
		//Log.d(TAG, "onBillingFinish,result=" + result);
	}

	@Override
	public void onQueryFinish(int code, HashMap arg1) {
		//Log.d(TAG, "onQueryFinish, status code = " + code);
		Message message = iapHandler.obtainMessage(IAPHandler.QUERY_FINISH);
		String result = "";
		// 姝ゆ璁㈣喘鐨刼rderID
		String orderID = null;
		// 鍟嗗搧鐨刾aycode
		String paycode = null;
		// 鍟嗗搧鐨勬湁鏁堟湡(浠呯璧佺被鍨嬪晢鍝佹湁鏁�)
		String leftday = null;
		if (code != PurchaseCode.QUERY_OK) {
			/**
			 * 鏌ヨ涓嶅埌鍟嗗搧璐拱鐨勭浉鍏充俊鎭�
			 */
			result = "result:" + Purchase.getReason(code);
		} else {
			/**
			 * 鏌ヨ鍒板晢鍝佺殑鐩稿叧淇℃伅銆�
			 * 姝ゆ椂浣犲彲浠ヨ幏寰楀晢鍝佺殑paycode锛宱rderid锛屼互鍙婂晢鍝佺殑鏈夋晥鏈焞eftday锛堜粎绉熻祦绫诲瀷鍟嗗搧鍙互杩斿洖锛�
			 */
			orderID = (String) arg1.get(OnPurchaseListener.ORDERID);
			if (orderID != null && orderID.trim().length() != 0) {
				result = result + "orderID:" + orderID;
			}
			paycode = (String) arg1.get(OnPurchaseListener.PAYCODE);
			if (paycode != null && paycode.trim().length() != 0) {
				result = result + ",paycode:" + paycode;
			}
			leftday = (String) arg1.get(OnPurchaseListener.LEFTDAY);
			if (leftday != null && leftday.trim().length() != 0) {
				result = result + ",leftday:" + leftday;
			}
		}
		//Log.d(TAG, "onQueryFinish,result=" + result);
	}

	

	@Override
	public void onUnsubscribeFinish(int code) {
		//Log.d(TAG, "onUnsubscribeFinish, status code = " + code+",reason="+Purchase.getReason(code));
	}
}

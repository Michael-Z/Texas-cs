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
		// 此次订购的orderID
		String orderID = null;
		// 商品的paycode
		String paycode = null;
		// 商品的有效期(仅租赁类型商品有效)
		String leftday = null;
		// 商品的交易 ID，用户可以根据这个交易ID，查询商品是否已经交易
		String tradeID = null;
		
		String ordertype = null;
		if (code == PurchaseCode.ORDER_OK || (code == PurchaseCode.AUTH_OK)) {
			/**
			 * 商品购买成功或者已经购买。 此时会返回商品的paycode，orderID,以及剩余时间(租赁类型商品)
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
			 * 表示订购失败。
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
		// 此次订购的orderID
		String orderID = null;
		// 商品的paycode
		String paycode = null;
		// 商品的有效期(仅租赁类型商品有效)
		String leftday = null;
		if (code != PurchaseCode.QUERY_OK) {
			/**
			 * 查询不到商品购买的相关信息
			 */
			result = "result:" + Purchase.getReason(code);
		} else {
			/**
			 * 查询到商品的相关信息。
			 * 此时你可以获得商品的paycode，orderid，以及商品的有效期leftday（仅租赁类型商品可以返回）
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

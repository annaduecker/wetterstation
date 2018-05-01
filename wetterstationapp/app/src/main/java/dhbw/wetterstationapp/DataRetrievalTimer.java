package dhbw.wetterstationapp;

import android.app.Activity;

import java.util.TimerTask;

public class DataRetrievalTimer extends TimerTask {
    HttpGetRequestClient atask;
    String[] params;

    public String[] getParams() {
        return params;
    }
    public void setParams(String[] params) {
        this.params = params;
    }

    public void run() {
        atask = new HttpGetRequestClient();
        atask.execute(params);
    }
}


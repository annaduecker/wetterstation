package dhbw.wetterstationapp;

import android.net.Uri;
import android.os.AsyncTask;
import android.util.Log;
import android.widget.Toast;

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.Closeable;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.ArrayList;

import javax.net.ssl.HttpsURLConnection;

public class HttpGetRequestClient extends AsyncTask<String, Void, String> {

    private HttpURLConnection httpConnection;
    private Object classType;

    @Override
    protected String doInBackground(String... params) {
        try {
            URL weatherstationEndpoint = new URL(params[0]);

            classType=params[1];
// Create connection
            httpConnection = (HttpURLConnection) weatherstationEndpoint.openConnection();
            httpConnection.setReadTimeout(10000);
            httpConnection.setConnectTimeout(15000);

            httpConnection.setDoInput(true);
            //httpConnection.setDoOutput(true);
            httpConnection.setInstanceFollowRedirects(false);
            httpConnection.setRequestMethod("GET");

            InputStream in = new BufferedInputStream(httpConnection.getInputStream());
             return readStream(in);
        } catch (Exception e) {
            Log.e("MainActivity", e.getMessage(), e);
        }
        finally {
            httpConnection.disconnect();
        }

        return null;
    }

    private String readStream(InputStream is) throws IOException {
        BufferedReader in = null;
        String inputLine;
        StringBuilder body;
        try {
            in = new BufferedReader(new InputStreamReader(is));

            body = new StringBuilder();

            while ((inputLine = in.readLine()) != null) {
                body.append(inputLine);
            }
            in.close();

            return body.toString();
        } catch(IOException ioe) {
            throw ioe;
        } finally {
            this.closeConnection(in);
        }
    }
    protected void closeConnection(Closeable closeable) {
        try {
            if( closeable != null ) {
                closeable.close();
            }
        } catch(IOException ex) {

        }
    }

    @Override
    protected void onPostExecute(String body) {
        try {
            if (classType == SensorDataCalculatedTouple.class.getName()) {
                MainActivity.getInstance().populateList(SensorDataCalculatedTouple.prepareData(body));
            } else if (classType == SensorDataChartTouple.class.getName()) {
                //MainActivity.getInstance().setGraphData(SensorDataChartTouple.prepareData(body));
                MainActivity.getInstance().setData(SensorDataChartTouple.prepareData(body));
            }


            Log.d("State", body);
        }
        catch (Exception e){
            e.printStackTrace();
            Toast.makeText( MainActivity.getInstance().getApplicationContext(), MainActivity.getInstance().getResources().getString(R.string.errorWhileLoadingData),Toast.LENGTH_SHORT).show();
        }
    }


}
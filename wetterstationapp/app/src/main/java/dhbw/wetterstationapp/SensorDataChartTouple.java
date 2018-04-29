package dhbw.wetterstationapp;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.text.DateFormat;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;

public class SensorDataChartTouple extends SensorDataTouple {

    public SensorDataChartTouple(int sensorId, float sensorValue, Date timestamp, String sensorName) {
        super(sensorId, sensorValue, timestamp, sensorName);
    }

    public SensorDataChartTouple(int sensorId, float sensorValue, Date timestamp) {
        super(sensorId, sensorValue, timestamp);
    }

    public SensorDataChartTouple() {

    }

    public static ArrayList<SensorDataTouple> prepareData(String body) {
        ArrayList<SensorDataTouple> sensorDataList=new ArrayList<SensorDataTouple>();

        try {
            // create the json array from String rules
            JSONArray jsonResponse = new JSONArray(body);
            // iterate over the rules
            for (int i = 0; i < jsonResponse.length(); i++) {
                JSONObject obj = (JSONObject) jsonResponse.get(i);
                int id= Integer.parseInt(obj.getString(JSONTOKEN_SENSORID));
                float sensorwert= Float.parseFloat(obj.getString(JSONTOKEN_SENSORWERT));
                String t=obj.getString(JSONTOKEN_DATUM);
              //  String t = "2018-04-08T09:51:23.000Z";
                SimpleDateFormat mFormat = new SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ss.SSSXXX");
                Date date = null;
                try {
                     date = mFormat.parse(t);
                } catch (ParseException e) {
                    e.printStackTrace();
                }
                sensorDataList.add(new SensorDataChartTouple(id,sensorwert,date));
            }
        } catch (JSONException e) {
            e.printStackTrace();
        }
        return sensorDataList;
    }
}

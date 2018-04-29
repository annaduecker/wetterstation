package dhbw.wetterstationapp;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;

public class SensorDataChartTouple extends SensorDataTouple {

    public SensorDataChartTouple(int sensorId, float sensorValue, float timestamp, String sensorName) {
        super(sensorId, sensorValue, timestamp, sensorName);
    }

    public SensorDataChartTouple(int sensorId, float sensorValue, float timestamp) {
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
                float datum=  Float.parseFloat(obj.getString(JSONTOKEN_DATUM));
                sensorDataList.add(new SensorDataChartTouple(id,sensorwert,datum));
            }
        } catch (JSONException e) {
            e.printStackTrace();
        }
        return sensorDataList;
    }
}

package dhbw.wetterstationapp;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.Date;

public class SensorDataCalculatedTouple extends SensorDataTouple {


    public SensorDataCalculatedTouple(String sensorName, int sensorId, float sensorValue, Date date) {
        super(sensorId, sensorValue,date,sensorName);
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
                float avg= Float.parseFloat(obj.getString(JSONTOKEN_AVG));
                String name= obj.getString(JSONTOKEN_SENSORNAME);
                Date date= null; //MainActivity.parseDate(obj.getString(JSONTOKEN_DATUM));
                sensorDataList.add(new SensorDataCalculatedTouple(name,id,avg,date));
            }
        } catch (JSONException e) {
            e.printStackTrace();
        }
        return sensorDataList;
    }
}

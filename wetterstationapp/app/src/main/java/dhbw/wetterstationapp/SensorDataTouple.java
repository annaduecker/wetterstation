package dhbw.wetterstationapp;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.Date;

public class SensorDataTouple {

    public static String JSONTOKEN_SENSORID;
    public static String JSONTOKEN_SENSORWERT;
    public static String JSONTOKEN_AVG;
    public static String JSONTOKEN_DATUM;
    public static String JSONTOKEN_SENSORNAME;

    static {
        JSONTOKEN_SENSORID = "sensorid";
        JSONTOKEN_AVG = "AVG";
        JSONTOKEN_SENSORWERT = "sensorwert";
        JSONTOKEN_DATUM = "datum";
        JSONTOKEN_SENSORNAME= "name";
    }




    int sensorId;
    float sensorValue;
    Date timestamp;
    String sensorName;

    public SensorDataTouple(int sensorId, float sensorValue, Date timestamp, String sensorName) {
        this.sensorId = sensorId;
        this.sensorValue = sensorValue;
        this.timestamp = timestamp;
        this.sensorName = sensorName;
    }

    public SensorDataTouple(int sensorId, float sensorValue, String sensorName) {
        this.sensorId = sensorId;
        this.sensorValue = sensorValue;
        this.sensorName = sensorName;
    }

    public SensorDataTouple(int sensorId, float sensorValue, Date timestamp) {
        this.sensorId = sensorId;
        this.sensorValue = sensorValue;
        this.timestamp = timestamp;
    }

    public SensorDataTouple(int sensorId, float sensorValue) {
        this.sensorId = sensorId;
        this.sensorValue = sensorValue;
    }

    public SensorDataTouple() {

    }

    public static  ArrayList<SensorDataTouple> prepareData(String body) {
        ArrayList<SensorDataTouple> sensorDataList=new ArrayList<SensorDataTouple>();
        try {
            // create the json array from String rules
            JSONArray jsonResponse = new JSONArray(body);
            // iterate over the rules
            for (int i = 0; i < jsonResponse.length(); i++) {
                JSONObject obj = (JSONObject) jsonResponse.get(i);
                int id= Integer.parseInt(obj.getString(JSONTOKEN_SENSORID));
                float sensorwert= Float.parseFloat(obj.getString(JSONTOKEN_SENSORWERT));
                Date datum=  null;
                sensorDataList.add(new SensorDataTouple(id,sensorwert,datum));
            }
        } catch (JSONException e) {
            e.printStackTrace();
        }
        return sensorDataList;
    }

    public Date getTimestamp() {
        return timestamp;
    }

    public void setTimestamp(Date timestamp) {
        this.timestamp = timestamp;
    }

    public int getSensorId() {
        return sensorId;
    }

    public void setSensorId(int sensorId) {
        this.sensorId = sensorId;
    }

    public float getSensorValue() {
        return sensorValue;
    }

    public void setSensorValue(float sensorValue) {
        this.sensorValue = sensorValue;
    }
    public String getSensorName() {
        return sensorName;
    }

    public void setSensorName(String sensorName) {
        this.sensorName = sensorName;
    }

}

package dhbw.wetterstationapp;

public class SensorDataTouple {

    public SensorDataTouple(int sensorId, float sensorValue, float timestamp) {
        this.sensorId = sensorId;
        this.sensorValue = sensorValue;
        this.timestamp = timestamp;
    }

    int sensorId;
    float sensorValue;

    public float getTimestamp() {
        return timestamp;
    }

    public void setTimestamp(float timestamp) {
        this.timestamp = timestamp;
    }

    float timestamp;


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


}

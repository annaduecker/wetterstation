package dhbw.wetterstationapp;

import com.jjoe64.graphview.series.DataPointInterface;

import java.io.Serializable;
import java.util.Date;

public class DMZDataPoint implements DataPointInterface,Serializable {
    private static final long serialVersionUID=1428263322645L;

    private double x;
    private double y;

    public DMZDataPoint(double x, double y) {
        this.x=x;
        this.y=y;
    }

    public DMZDataPoint(Date x, double y) {
        this.x = x.getTime();
        this.y = y;
    }

    @Override
    public double getX() {
        return x;
    }

    @Override
    public double getY() {
        return y;
    }

    @Override
    public String toString() {
        return "["+x+"/"+y+"]";
    }
}

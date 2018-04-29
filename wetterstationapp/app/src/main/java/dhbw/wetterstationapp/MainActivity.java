package dhbw.wetterstationapp;

import android.content.Context;
import android.graphics.Color;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.design.widget.BottomNavigationView;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.MenuItem;
import android.view.View;
import android.webkit.ConsoleMessage;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.TextView;

import com.github.mikephil.charting.charts.BarChart;
import com.github.mikephil.charting.charts.LineChart;
import com.github.mikephil.charting.components.AxisBase;
import com.github.mikephil.charting.components.Legend;
import com.github.mikephil.charting.components.XAxis;
import com.github.mikephil.charting.components.YAxis;
import com.github.mikephil.charting.data.BarData;
import com.github.mikephil.charting.data.BarDataSet;
import com.github.mikephil.charting.data.BarEntry;
import com.github.mikephil.charting.data.Entry;
import com.github.mikephil.charting.data.LineData;
import com.github.mikephil.charting.data.LineDataSet;
import com.github.mikephil.charting.formatter.IAxisValueFormatter;
import com.github.mikephil.charting.interfaces.datasets.ILineDataSet;
import com.github.mikephil.charting.utils.ColorTemplate;
import com.jjoe64.graphview.GraphView;
import com.jjoe64.graphview.helper.DateAsXAxisLabelFormatter;
import com.jjoe64.graphview.series.DataPoint;
import com.jjoe64.graphview.series.LineGraphSeries;

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.Closeable;
import java.io.Console;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;
import java.text.SimpleDateFormat;
import java.time.LocalDateTime;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Random;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.TimeUnit;

public class MainActivity extends AppCompatActivity {

    public static final String FIRST_COLUMN = "First";
    public static final String SECOND_COLUMN = "Second";
    public static final String THIRD_COLUMN = "Third";
    public static final String FOURTH_COLUMN = "Fourth";
    private LineChart chart;
    private BarChart bChart;
    private  static ArrayList<HashMap> list;
    public  ListView lview;
    private GraphView graph;
    private BottomNavigationView.OnNavigationItemSelectedListener mOnNavigationItemSelectedListener
            = new BottomNavigationView.OnNavigationItemSelectedListener() {

        @Override
        public boolean onNavigationItemSelected(@NonNull MenuItem item) {
            switch (item.getItemId()) {
                case R.id.navigation_home:
                    chart.setVisibility(View.INVISIBLE);
                    bChart.setVisibility(View.INVISIBLE);
                    graph.setVisibility(View.INVISIBLE);
                    lview.setVisibility(View.VISIBLE);
                    return true;
                case R.id.navigation_dashboard:
                    chart.setVisibility(View.VISIBLE);
                    bChart.setVisibility(View.INVISIBLE);
                    graph.setVisibility(View.INVISIBLE);
                    lview.setVisibility(View.INVISIBLE);
                    return true;
                case R.id.navigation_notifications:
                    bChart.setVisibility(View.VISIBLE);
                    chart.setVisibility(View.INVISIBLE);
                    graph.setVisibility(View.INVISIBLE);
                    lview.setVisibility(View.INVISIBLE);
                    return true;
            }
            return false;
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        String[] params = new String[]{"http://192.168.0.87:3000/weatherStationServer/sensor/calculated/avg",  SensorDataCalculatedTouple.class.getName()};
        HttpGetRequestClient httpGetRequestClient= new HttpGetRequestClient();
        httpGetRequestClient.setMain(this);
        httpGetRequestClient.execute(params);


        Calendar calendar = Calendar.getInstance();
        Date d1 = calendar.getTime();
        calendar.add(Calendar.DATE, 1);
        Date d2 = calendar.getTime();
        calendar.add(Calendar.DATE, 1);
        Date d3 = calendar.getTime();

        graph = (GraphView) findViewById(R.id.graph);

// you can directly pass Date objects to DataPoint-Constructor
// this will convert the Date to double via Date#getTime()
        LineGraphSeries<DataPoint> series = new LineGraphSeries<>(new DataPoint[] {
                new DataPoint(d1, 1),
                new DataPoint(d2, 5),
                new DataPoint(d3, 3)
        });

        series.setDrawDataPoints(true);
        series.setDataPointsRadius(10);
        series.setThickness(8);
        graph.addSeries(series);

// set date label formatter
        graph.getGridLabelRenderer().setLabelFormatter(new DateAsXAxisLabelFormatter(this));
        graph.getGridLabelRenderer().setNumHorizontalLabels(3); // only 4 because of the space

// set manual x bounds to have nice steps
        graph.getViewport().setMinX(d1.getTime());
        graph.getViewport().setMaxX(d3.getTime());
        graph.getViewport().setXAxisBoundsManual(true);

// as we use dates as labels, the human rounding to nice readable numbers
// is not necessary
        graph.getGridLabelRenderer().setHumanRounding(false);


        //listviewAdapter adapter = new listviewAdapter(this, list);
        //lview.setAdapter(adapter);



        chart = (LineChart) findViewById(R.id.linechart);
        bChart = (BarChart) findViewById(R.id.barchart);
        List<BarEntry> entries = new ArrayList<>();
        entries.add(new BarEntry(0f, 30f));
        entries.add(new BarEntry(1f, 80f));
        entries.add(new BarEntry(2f, 60f));
        entries.add(new BarEntry(3f, 50f));
        // gap of 2f
        entries.add(new BarEntry(5f, 70f));
        entries.add(new BarEntry(6f, 60f));

        BarDataSet set = new BarDataSet(entries, "BarDataSet");
        BarData bData = new BarData(set);
        bData.setBarWidth(0.9f); // set custom bar width
        bChart.setData(bData);
        bChart.setFitBars(true); // make the x-axis fit exactly all bars
        bChart.invalidate(); // refresh


        chart.setVisibility(View.INVISIBLE);
        bChart.setVisibility(View.INVISIBLE);
        ArrayList <SensorDataTouple> dataObjects=  new ArrayList<SensorDataTouple>();
        ArrayList <SensorDataTouple> dataObjects2=  new ArrayList<SensorDataTouple>();

        dataObjects.add(new SensorDataTouple (1,75,0));
        dataObjects.add(new SensorDataTouple (2,85,25200));
        dataObjects.add(new SensorDataTouple (3,95,61200));
        dataObjects.add(new SensorDataTouple (4,65,82800));

        dataObjects2.add(new SensorDataTouple (1,15,0));
        dataObjects2.add(new SensorDataTouple (2,35,25200));
        dataObjects2.add(new SensorDataTouple (3,45,61200));
        dataObjects2.add(new SensorDataTouple (4,50,82800));
        List<Entry> sensorList1 = new ArrayList<Entry>();
        List<Entry> sensorList2 = new ArrayList<Entry>();
        for (SensorDataTouple data : dataObjects) {
            // turn your data into Entry objects
            sensorList1.add(new Entry(data.getTimestamp(), data.getSensorValue()));
        }
        for (SensorDataTouple data : dataObjects2) {
            // turn your data into Entry objects
            sensorList2.add(new Entry(data.getTimestamp(), data.getSensorValue()));
        }

        LineDataSet setComp1 = new LineDataSet(sensorList1, "Sensor1 1");
        setComp1.setAxisDependency(YAxis.AxisDependency.LEFT);
        LineDataSet setComp2 = new LineDataSet(sensorList2, "Sensor 2");
        setComp2.setAxisDependency(YAxis.AxisDependency.LEFT);
        /*dataSet.setColor(Color.parseColor("white"));
        dataSet.setValueTextColor(Color.parseColor("black")); // styling, ...
        LineData lineData = new LineData(dataSet);
        chart.setData(lineData);
        chart.invalidate(); // refresh*/
        List<ILineDataSet> dataSets = new ArrayList<ILineDataSet>();
        dataSets.add(setComp1);
        dataSets.add(setComp2);



       LineData data = new LineData(dataSets);
       // chart.setData(data);

        setData(100, 30);
        chart.invalidate();

        // get the legend (only possible after setting data)
        Legend l = chart.getLegend();
        l.setEnabled(false);

        XAxis xAxis = chart.getXAxis();
        xAxis.setPosition(XAxis.XAxisPosition.TOP_INSIDE);

        xAxis.setTextSize(10f);
        xAxis.setTextColor(Color.WHITE);
        xAxis.setDrawAxisLine(false);
        xAxis.setDrawGridLines(true);
        xAxis.setTextColor(Color.rgb(255, 192, 56));
        xAxis.setCenterAxisLabels(true);
        xAxis.setGranularity(1f); // one hour
        xAxis.setValueFormatter(new IAxisValueFormatter() {

            private SimpleDateFormat mFormat = new SimpleDateFormat("dd MMM HH:mm");

            @Override
            public String getFormattedValue(float value, AxisBase axis) {

                long millis = TimeUnit.HOURS.toMillis((long) value);
                return mFormat.format(new Date(millis));
            }
        });

        YAxis leftAxis = chart.getAxisLeft();
        leftAxis.setPosition(YAxis.YAxisLabelPosition.INSIDE_CHART);

        leftAxis.setTextColor(ColorTemplate.getHoloBlue());
        leftAxis.setDrawGridLines(true);
        leftAxis.setGranularityEnabled(true);
        leftAxis.setAxisMinimum(0f);
        leftAxis.setAxisMaximum(50);
        leftAxis.setYOffset(-45f);
        leftAxis.setTextColor(Color.rgb(255, 192, 56));

        YAxis rightAxis = chart.getAxisRight();
        rightAxis.setEnabled(false);

        BottomNavigationView navigation = (BottomNavigationView) findViewById(R.id.navigation);
        navigation.setOnNavigationItemSelectedListener(mOnNavigationItemSelectedListener);
    }
    private void setData(int count, float range) {

        // now in hours
        long now = TimeUnit.MILLISECONDS.toHours(System.currentTimeMillis());

        ArrayList<Entry> values = new ArrayList<Entry>();

        float from = now;

        // count = hours
        float to = now + count;

        // increment by 1 hour
        for (float x = from; x < to; x++) {

            Random rn = new Random();

            float y = rn.nextInt((int)range) + 1;
            values.add(new Entry(x, y)); // add one entry per hour
        }

        // create a dataset and give it a type
        LineDataSet set1 = new LineDataSet(values, "DataSet 1");
        set1.setAxisDependency(YAxis.AxisDependency.LEFT);
        set1.setColor(ColorTemplate.getHoloBlue());
        set1.setValueTextColor(ColorTemplate.getHoloBlue());
        set1.setLineWidth(1.5f);
        set1.setDrawCircles(false);
        set1.setDrawValues(false);
        set1.setFillAlpha(65);
        set1.setFillColor(ColorTemplate.getHoloBlue());
        set1.setHighLightColor(Color.rgb(244, 117, 117));
        set1.setDrawCircleHole(false);

        // create a data object with the datasets
        LineData data = new LineData(set1);
        data.setValueTextColor(Color.WHITE);
        data.setValueTextSize(9f);

        // set data
        chart.setData(data);
    }

    public void populateList(ArrayList<SensorDataTouple> sensorData) {

        list = new ArrayList<HashMap>();
        HashMap tmp99 = new HashMap();
        tmp99.put(FIRST_COLUMN,"Sensor");
        tmp99.put(SECOND_COLUMN, "Avg (24h)");
        list.add(tmp99);
        for( SensorDataTouple sample: sensorData )
        {
            HashMap tmp = new HashMap();
            tmp.put(FIRST_COLUMN,String.valueOf(sample.getSensorName()) );
            tmp.put(SECOND_COLUMN, String.valueOf(sample.getSensorValue()));
            list.add(tmp);
        }
        lview = (ListView) findViewById(R.id.listview);
        listviewAdapter adapter = new listviewAdapter(this, list);
        lview.setAdapter(adapter);

    }
}

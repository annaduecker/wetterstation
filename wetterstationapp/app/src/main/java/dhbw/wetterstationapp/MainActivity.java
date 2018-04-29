package dhbw.wetterstationapp;

import android.content.pm.ActivityInfo;
import android.graphics.Color;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.design.widget.BottomNavigationView;
import android.support.design.widget.NavigationView;
import android.support.v4.view.GravityCompat;
import android.support.v4.widget.DrawerLayout;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

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


import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.time.Duration;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;
import java.util.GregorianCalendar;
import java.util.HashMap;
import java.util.List;
import java.util.Random;
import java.util.TimeZone;
import java.util.Timer;
import java.util.concurrent.TimeUnit;

import static dhbw.wetterstationapp.Constant.LISTVIEW_DATE;
import static dhbw.wetterstationapp.Constant.LISTVIEW_SENSORID;
import static dhbw.wetterstationapp.Constant.LISTVIEW_SENSORNAME;
import static dhbw.wetterstationapp.Constant.LISTVIEW_VALUE;

public class MainActivity extends AppCompatActivity {

    private LineChart chart;
    private BarChart bChart;
    private  static ArrayList<HashMap> list;
    public  ListView lview;
    private GraphView graph;
    private TextView graphText;
    private DrawerLayout mDrawerLayout;
    private static MainActivity instance;

    private Timer calculatedValueTimer = new Timer();
    private DataRetrievalTimer calculatedDataRetrievalTimer = new DataRetrievalTimer();
    private Timer diagramValueTimer = new Timer();
    private DataRetrievalTimer diagramDataRetrievalTimer = new DataRetrievalTimer();
    private final String[] CALCULATED_PARAMS = new String[]{"http://192.168.0.87:3000/weatherStationServer/sensor/calculated/avg",  SensorDataCalculatedTouple.class.getName()};
    private final String[] SENOSORDATA_PARAMS = new String[]{"http://192.168.0.87:3000/weatherStationServer/sensor/",  SensorDataChartTouple.class.getName()};



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
        setInstance(this);





        TimeZone tzone = TimeZone.getTimeZone("Europe/Berlin");
        tzone.setDefault(tzone);

        setContentView(R.layout.activity_main);
        Toolbar toolbar = findViewById(R.id.toolbar);
        graphText = findViewById(R.id.graphText);
        setSupportActionBar(toolbar);
        ActionBar actionbar = getSupportActionBar();
        actionbar.setDisplayHomeAsUpEnabled(true);
        actionbar.setHomeAsUpIndicator(R.drawable.ic_menu_black_24dp);
        mDrawerLayout = findViewById(R.id.drawer_layout);

        NavigationView navigationView = findViewById(R.id.nav_view);
        navigationView.setNavigationItemSelectedListener(
                new NavigationView.OnNavigationItemSelectedListener() {
                    @Override
                    public boolean onNavigationItemSelected(MenuItem menuItem) {
                        // set item as selected to persist highlight
                        menuItem.setChecked(true);
                        // close drawer when item is tapped
                        mDrawerLayout.closeDrawers();
                        switch (menuItem.getItemId()) {
                            case R.id.nav_refresh:
                                calculatedValueTimer.cancel();
                                calculatedValueTimer.purge();
                                setCalculatedValueTimer(new Timer());
                                setCalculatedDataRetrievalTimer(new DataRetrievalTimer());
                                calculatedDataRetrievalTimer.setParams(CALCULATED_PARAMS);
                                calculatedValueTimer.schedule(calculatedDataRetrievalTimer, 0, 50000); //execute in every 50000 ms
                                Toast.makeText( getApplicationContext(), getResources().getString(R.string.data_refreshed),Toast.LENGTH_SHORT).show();
                                break;

                            case R.id.nav_home:
                                bChart.setVisibility(View.INVISIBLE);
                                graph.setVisibility(View.INVISIBLE);
                                chart.setVisibility(View.INVISIBLE);
                                lview.setVisibility(View.VISIBLE);
                                graphText.setVisibility(View.INVISIBLE);
                                getInstance().setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
                             break;

                                // Add code here to update the UI based on the item selected
                            // For example, swap UI fragments here
                        }
                        return true;
                    }
                });

        calculatedDataRetrievalTimer.setParams(CALCULATED_PARAMS);
        calculatedValueTimer.schedule(calculatedDataRetrievalTimer, 0, 50000); //execute in every 50000 ms



// you can directly pass Date objects to DataPoint-Constructor
// this will convert the Date to double via Date#getTime()



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

        dataObjects.add(new SensorDataTouple (1,75,new Date()));
        dataObjects.add(new SensorDataTouple (2,85,new Date()));
        dataObjects.add(new SensorDataTouple (3,95,new Date()));
        dataObjects.add(new SensorDataTouple (4,65,new Date()));

        dataObjects2.add(new SensorDataTouple (1,15,new Date()));
        dataObjects2.add(new SensorDataTouple (2,35,new Date()));
        dataObjects2.add(new SensorDataTouple (3,45,new Date()));
        dataObjects2.add(new SensorDataTouple (4,50,new Date()));
        List<Entry> sensorList1 = new ArrayList<Entry>();
        List<Entry> sensorList2 = new ArrayList<Entry>();
        for (SensorDataTouple data : dataObjects) {
            // turn your data into Entry objects
            sensorList1.add(new Entry(new Float(3), data.getSensorValue()));
        }
        for (SensorDataTouple data : dataObjects2) {
            // turn your data into Entry objects
            sensorList2.add(new Entry(new Float(5), data.getSensorValue()));
        }

        LineDataSet setComp1 = new LineDataSet(sensorList1, "Sensor1 1");
        setComp1.setAxisDependency(YAxis.AxisDependency.LEFT);
        LineDataSet setComp2 = new LineDataSet(sensorList2, "Sensor 2");
        setComp2.setAxisDependency(YAxis.AxisDependency.LEFT);
        //dataSet.setColor(Color.parseColor("white"));
        //dataSet.setValueTextColor(Color.parseColor("black")); // styling, ...
        //LineData lineData = new LineData(dataSet);
        //chart.setData(lineData);
       // chart.invalidate(); // refresh
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

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case android.R.id.home:
                mDrawerLayout.openDrawer(GravityCompat.START);
                return true;
        }
        return super.onOptionsItemSelected(item);
    }

    public void setGraphData(ArrayList<SensorDataTouple> sensorData){

        DataPoint[] values = new DataPoint[sensorData.size()];
        for (int i=0; i<sensorData.size(); i++) {
            DataPoint v = new DataPoint(sensorData.get(i).getTimestamp(), sensorData.get(i).getSensorValue());

            values[i] = v;
        }
       LineGraphSeries<DataPoint> series = new LineGraphSeries<>(values);

        graph = (GraphView) findViewById(R.id.graph);

        series.setDrawDataPoints(true);
        series.setDataPointsRadius(10);
        series.setThickness(8);
        graph.removeAllSeries();
        graph.addSeries(series);

// set date label formatter
        graph.getGridLabelRenderer().setLabelFormatter(new DateAsXAxisLabelFormatter(this,new SimpleDateFormat("dd MMM HH:mm")));
        graph.getGridLabelRenderer().setNumHorizontalLabels(3); // only 4 because of the space

// set manual x bounds to have nice steps
        graph.getViewport().setMinX(sensorData.get(0).getTimestamp().getTime());
        graph.getViewport().setMaxX(sensorData.get(2).getTimestamp().getTime());
        graph.getViewport().setXAxisBoundsManual(true);

// as we use dates as labels, the human rounding to nice readable numbers
// is not necessary
        graph.getGridLabelRenderer().setHumanRounding(false);

        //getInstance().setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
    }

    public void populateList(ArrayList<SensorDataTouple> sensorData) {

        list = new ArrayList<HashMap>();


        SimpleDateFormat formatter = new SimpleDateFormat("dd MMM YY HH:mm");

        Calendar calendar = Calendar.getInstance(TimeZone.getTimeZone("Europe/Berlin"));
        Date currentDate = calendar.getTime();
        String time= formatter.format(currentDate);

        for( SensorDataTouple sample: sensorData )
        {
            HashMap tmp = new HashMap();
            tmp.put(LISTVIEW_SENSORNAME,String.valueOf(sample.getSensorName()) );
            tmp.put(LISTVIEW_VALUE, String.valueOf(sample.getSensorValue()));
            tmp.put(LISTVIEW_SENSORID, String.valueOf(sample.getSensorId()));
            tmp.put(LISTVIEW_DATE,time);
            list.add(tmp);
        }

        listviewAdapter adapter = new listviewAdapter(this, list);
        lview = (ListView) findViewById(R.id.listview);
        lview.setAdapter(adapter);
        lview.setOnItemClickListener(new android.widget.AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                HashMap map= (HashMap) lview.getItemAtPosition(position);
                int item= Integer.parseInt(String.valueOf(map.get(LISTVIEW_SENSORID)));
                graphText = findViewById(R.id.graphText);
                graph = findViewById(R.id.graph);
                diagramValueTimer.cancel();
                diagramValueTimer.purge();
                setDiagramValueTimer(new Timer());
                setDiagramDataRetrievalTimer(new DataRetrievalTimer());
                String[] param = getSENOSORDATA_PARAMS();
                getDiagramDataRetrievalTimer().setParams(new String[]{param[0]+item, param[1]});
                getDiagramValueTimer().schedule(getDiagramDataRetrievalTimer(), 0, 150000); //execute in every 50000 ms
                graph.setVisibility(View.VISIBLE);
                lview.setVisibility(View.INVISIBLE);
                graphText.setVisibility(View.VISIBLE);
                graphText.setText(R.string.graphText);
                graphText.setText(graphText.getText()+": "+map.get(LISTVIEW_SENSORNAME)+ "  AVG:"+ map.get(LISTVIEW_VALUE));
                Toast.makeText(getInstance(),"You selected : " + map.get(LISTVIEW_SENSORNAME),Toast.LENGTH_SHORT).show();
            }
        });

    }
    public Timer getCalculatedValueTimer() {
        return calculatedValueTimer;
    }

    public void setCalculatedValueTimer(Timer calculatedValueTimer) {
        this.calculatedValueTimer = calculatedValueTimer;
    }
    public DataRetrievalTimer getCalculatedDataRetrievalTimer() {
        return calculatedDataRetrievalTimer;
    }

    public void setCalculatedDataRetrievalTimer(DataRetrievalTimer calculatedDataRetrievalTimer) {
        this.calculatedDataRetrievalTimer = calculatedDataRetrievalTimer;
    }

    public static MainActivity getInstance() {
        return instance;
    }

    private static void setInstance(MainActivity instance) {
        MainActivity.instance = instance;
    }

    public Timer getDiagramValueTimer() {
        return diagramValueTimer;
    }

    public void setDiagramValueTimer(Timer diagramValueTimer) {
        this.diagramValueTimer = diagramValueTimer;
    }

    public DataRetrievalTimer getDiagramDataRetrievalTimer() {
        return diagramDataRetrievalTimer;
    }

    public void setDiagramDataRetrievalTimer(DataRetrievalTimer diagramDataRetrievalTimer) {
        this.diagramDataRetrievalTimer = diagramDataRetrievalTimer;
    }

    public String[] getSENOSORDATA_PARAMS() {
        return SENOSORDATA_PARAMS;
    }
}

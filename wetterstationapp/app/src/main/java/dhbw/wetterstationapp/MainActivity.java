package dhbw.wetterstationapp;

import android.content.pm.ActivityInfo;
import android.graphics.Color;
import android.graphics.Typeface;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.design.widget.BottomNavigationView;
import android.support.design.widget.NavigationView;
import android.support.v4.view.GravityCompat;
import android.support.v4.widget.DrawerLayout;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import com.github.mikephil.charting.charts.BarChart;
import com.github.mikephil.charting.charts.LineChart;
import com.github.mikephil.charting.components.AxisBase;
import com.github.mikephil.charting.components.Description;
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
import com.github.mikephil.charting.utils.EntryXComparator;
import com.jjoe64.graphview.GraphView;
import com.jjoe64.graphview.helper.DateAsXAxisLabelFormatter;
import com.jjoe64.graphview.series.DataPoint;
import com.jjoe64.graphview.series.LineGraphSeries;


import org.w3c.dom.Text;

import java.text.DateFormat;
import java.text.ParseException;

import java.text.SimpleDateFormat;
import java.time.Duration;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Collections;
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
    private  static ArrayList<HashMap> list;
    public  ListView lview;
    private GraphView graph;
    private TextView graphText;
    private DrawerLayout mDrawerLayout;
    private static MainActivity instance;
    private TextView dateToday;
    private TextView emptyList;
    private  MenuItem nav_toggle;
    private Timer calculatedValueTimer = new Timer();
    private DataRetrievalTimer calculatedDataRetrievalTimer = new DataRetrievalTimer();
    private Timer diagramValueTimer = new Timer();
    private DataRetrievalTimer diagramDataRetrievalTimer = new DataRetrievalTimer();
    private final String[] CALCULATED_PARAMS = new String[]{"http://wetterstation.westeurope.cloudapp.azure.com:3000/weatherStationServer/sensor/calculated/avg",  SensorDataCalculatedTouple.class.getName()};
    private final String[] SINGLEDATA_PARAMS = new String[]{"http://wetterstation.westeurope.cloudapp.azure.com:3000/weatherStationServer/sensor/calculated/avg/last",  SensorDataCalculatedTouple.class.getName()};
    private final String[] SENOSORDATA_PARAMS = new String[]{"http://wetterstation.westeurope.cloudapp.azure.com:3000/weatherStationServer/sensor/",  SensorDataChartTouple.class.getName()};

    public boolean isAVG() {
        return isAVG;
    }

    public void setAVG(boolean AVG) {
        NavigationView navigationView = findViewById(R.id.nav_view);
        // get menu from navigationView
        Menu menu = navigationView.getMenu();

        // find MenuItem you want to change
        MenuItem nav_toggle = menu.findItem(R.id.nav_toggle);
       if(AVG){
            nav_toggle.setTitle(R.string.toggled);
            Toast.makeText( getApplicationContext(), getResources().getString(R.string.toggled),Toast.LENGTH_SHORT).show();

        }
        else {
            nav_toggle.setTitle(R.string.untoggled);
            Toast.makeText( getApplicationContext(), getResources().getString(R.string.untoggled),Toast.LENGTH_SHORT).show();
        }

        isAVG = AVG;
        getSensorData(10000);

    }

    private boolean isAVG=true;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setInstance(this);

        TimeZone tzone = TimeZone.getTimeZone("Europe/Berlin");
        tzone.setDefault(tzone);

        setContentView(R.layout.activity_main);
        Toolbar toolbar = findViewById(R.id.toolbar);
         dateToday= findViewById(R.id.dateToday);
        TaskScheduler timer = new TaskScheduler();
        timer.scheduleAtFixedRate(new Runnable() {
            @Override
            public void run() {
                dateToday.setText( getResources().getString(R.string.dateToday)+" "+getDate(null,true));
            }
        },7000);


        emptyList= findViewById(R.id.emptyList);

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

                        try {


                        switch (menuItem.getItemId()) {
                            case R.id.nav_refresh:

                                getSensorData(10000);
                                Toast.makeText( getApplicationContext(), getResources().getString(R.string.data_refreshed),Toast.LENGTH_SHORT).show();
                                break;

                            case R.id.nav_home:
                                graph.setVisibility(View.INVISIBLE);
                                chart.setVisibility(View.INVISIBLE);
                                lview.setVisibility(View.VISIBLE);
                                dateToday.setVisibility(View.VISIBLE);
                                graphText.setVisibility(View.INVISIBLE);
                                //getInstance().setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
                             break;
                            case R.id.nav_toggle:
                                setAVG(!isAVG());

                                //getInstance().setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
                                break;

                        }
                        }
                        catch (Exception e){

                        }
                        return true;
                    }
                });



        getSensorData(15000);
        chart = (LineChart) findViewById(R.id.linechart);
                lview = (ListView) findViewById(R.id.listview);
        if(lview.getAdapter() == null){
            emptyList.setVisibility(View.VISIBLE);
        }

    }

    private void getSensorData(int period){
        calculatedValueTimer.cancel();
        calculatedValueTimer.purge();
        setCalculatedValueTimer(new Timer());
        setCalculatedDataRetrievalTimer(new DataRetrievalTimer());
        if(isAVG) {
            calculatedDataRetrievalTimer.setParams(CALCULATED_PARAMS);
            calculatedValueTimer.schedule(calculatedDataRetrievalTimer, 0, 60000); //execute in every 50000 ms
        }
        else{
            calculatedDataRetrievalTimer.setParams(SINGLEDATA_PARAMS);
            calculatedValueTimer.schedule(calculatedDataRetrievalTimer, 0, period); //execute in every 50000 ms
        }
    }



    public void setData(ArrayList<SensorDataTouple> sensorData) {

        // now in hours
       // long now = TimeUnit.MILLISECONDS.toHours(System.currentTimeMillis());
        int count=30;
        int range=50;
        ArrayList<Entry> values = new ArrayList<Entry>();

        //float from = now;

        // count = hours
        //float to = now + count;

        // increment by 1 hour
       /* for (float x = from; x < to; x++) {
            Random rn=new Random();
            float y = rn.nextInt(range+1);
            values.add(new Entry(x, y)); // add one entry per hour
        }*/

        float min=0,max=0;
        // increment by 1 hour
        String sensorName="";
        for( SensorDataTouple sample: sensorData )
        {

            long now = TimeUnit.MILLISECONDS.toHours(sample.getTimestamp().getTime());
            float x = now;
            float y=Float.parseFloat(String.valueOf(sample.getSensorValue()));
             sensorName=sample.getSensorName();
            if(max<y)
                max=y;
            values.add(new Entry(x,y)); // add one entry per hour
        }

        Collections.sort(values, new EntryXComparator());
        // create a dataset and give it a type
        LineDataSet set1 = new LineDataSet(values, "DataSet 1");
        set1.setAxisDependency(YAxis.AxisDependency.LEFT);
        set1.setColor(ColorTemplate.getHoloBlue());
        set1.setValueTextColor(ColorTemplate.getHoloBlue());
        set1.setLineWidth(1.5f);
        set1.setDrawCircles(true);
        set1.setDrawValues(true);
        set1.setFillAlpha(65);
        set1.setFillColor(ColorTemplate.getHoloBlue());
        set1.setHighLightColor(Color.rgb(244, 117, 117));
        set1.setDrawCircleHole(false);
        set1.setCircleRadius(3f);
        set1.setDrawCircleHole(true);
        //set1.setDrawFilled(true);
        //set1.setFillAlpha(110);

        // create a data object with the datasets
        LineData data = new LineData(set1);
        data.setValueTextColor(Color.BLACK);
        data.setValueTextSize(9f);

        // set data
        chart.setData(data);
        chart.invalidate();

        // get the legend (only possible after setting data)
        Legend l = chart.getLegend();
        l.setEnabled(false);

        XAxis xAxis = chart.getXAxis();
        xAxis.setPosition(XAxis.XAxisPosition.TOP_INSIDE);
        xAxis.setTypeface(Typeface.DEFAULT);
        xAxis.setTextSize(10f);
        xAxis.setTextColor(Color.WHITE);
        xAxis.setDrawAxisLine(false);
        xAxis.setDrawGridLines(true);
        xAxis.setTextColor(Color.rgb(255, 192, 56));
        xAxis.setCenterAxisLabels(true);
        xAxis.setGranularity(1f); // one hour
        xAxis.setValueFormatter(new IAxisValueFormatter() {

            private SimpleDateFormat mFormat = new SimpleDateFormat("HH:mm");

            @Override
            public String getFormattedValue(float value, AxisBase axis) {

                long millis = TimeUnit.HOURS.toMillis((long) value);
                return mFormat.format(new Date(millis));
            }
        });

        YAxis leftAxis = chart.getAxisLeft();
        leftAxis.setPosition(YAxis.YAxisLabelPosition.INSIDE_CHART);
        leftAxis.setTypeface(Typeface.DEFAULT);
        leftAxis.setTextColor(ColorTemplate.getHoloBlue());
        leftAxis.setDrawGridLines(true);
        leftAxis.setGranularityEnabled(true);
        leftAxis.setAxisMinimum(0f);
        leftAxis.setAxisMaximum(max+5);
        leftAxis.setYOffset(-9f);
        leftAxis.setTextColor(Color.rgb(255, 192, 56));

        YAxis rightAxis = chart.getAxisRight();
        rightAxis.setEnabled(false);
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

    public static Date parseDate(String dateString){
        SimpleDateFormat formatter = new SimpleDateFormat("dd-MM-YY HH:mm:ss");
        try {

            Calendar calendar = Calendar.getInstance(TimeZone.getTimeZone("Europe/Berlin"));
            calendar.setTime( formatter.parse(dateString));
            Date date = calendar.getTime();
            return  date;
        } catch (ParseException e) {
            e.printStackTrace();
            return null;
        }


    }

    private String getDate(Date date,boolean today){
        SimpleDateFormat formatter;
        if(isAVG || today) {
             formatter = new SimpleDateFormat("dd.MM.yyyy");
        }
        else
             formatter = new SimpleDateFormat("dd.MM.yyyy HH:mm:ss");
        Date currentDate;
        Calendar calendar = Calendar.getInstance(TimeZone.getTimeZone("Europe/Berlin"));

        if(date!=null){
            currentDate = date;
        }
        else
            currentDate = calendar.getTime();

        return formatter.format(currentDate);
    }

    public void populateList(ArrayList<SensorDataTouple> sensorData) {

        list = new ArrayList<HashMap>();




        for( SensorDataTouple sample: sensorData )
        {
            HashMap tmp = new HashMap();
            tmp.put(LISTVIEW_SENSORNAME,String.valueOf(sample.getSensorName()) );
            tmp.put(LISTVIEW_VALUE, String.valueOf(sample.getSensorValue()));
            tmp.put(LISTVIEW_SENSORID, String.valueOf(sample.getSensorId()));
            tmp.put(LISTVIEW_DATE,getDate(sample.getTimestamp(),false));
            list.add(tmp);
        }
        emptyList.setVisibility(View.INVISIBLE);
        listviewAdapter adapter = new listviewAdapter(this, list);
        lview = (ListView) findViewById(R.id.listview);
        lview.setAdapter(adapter);
        lview.setOnItemClickListener(new android.widget.AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {

                HashMap map= (HashMap) lview.getItemAtPosition(position);
                if(Integer.parseInt(String.valueOf(map.get(LISTVIEW_SENSORID)))==6){
                    Toast.makeText( getApplicationContext(), getResources().getString(R.string.windDirection)+" "+listviewAdapter.getWindDirection(Math.round((Float.parseFloat(map.get(LISTVIEW_VALUE).toString())))),Toast.LENGTH_SHORT).show();
                    return;
                }
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
               // graph.setVisibility(View.VISIBLE);
                chart.setVisibility(View.VISIBLE);
                lview.setVisibility(View.INVISIBLE);
                graphText.setVisibility(View.VISIBLE);
                emptyList.setVisibility(View.INVISIBLE);
                dateToday.setVisibility(View.INVISIBLE);
                graphText.setText(R.string.graphText);
                graphText.setText(map.get(LISTVIEW_SENSORNAME)+ "  AVG:"+ map.get(LISTVIEW_VALUE));
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

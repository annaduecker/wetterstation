package dhbw.wetterstationapp;


import java.util.ArrayList;
import java.util.HashMap;
import android.app.Activity;
import android.media.Image;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.TextView;

import static dhbw.wetterstationapp.Constant.LISTVIEW_DATE;
import static dhbw.wetterstationapp.Constant.LISTVIEW_IMAGEVIEW;
import static dhbw.wetterstationapp.Constant.LISTVIEW_SENSORNAME;
import static dhbw.wetterstationapp.Constant.LISTVIEW_VALUE;
import static dhbw.wetterstationapp.Constant.LISTVIEW_SENSORID;

public class listviewAdapter extends BaseAdapter
{

    public ArrayList<HashMap> list;
    Activity activity;

    public listviewAdapter(Activity activity, ArrayList<HashMap> list) {
        super();
        this.activity = activity;
        this.list = list;
    }

    @Override
    public int getCount() {
        // TODO Auto-generated method stub
        return list.size();
    }

    @Override
    public Object getItem(int position) {
        // TODO Auto-generated method stub
        return list.get(position);
    }

    @Override
    public long getItemId(int position) {
        // TODO Auto-generated method stub
        return 0;
    }

    private class ViewHolder {
        TextView txtSensorName;
        TextView txtValue;
        TextView txtDate;
        ImageView imageView;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        // TODO Auto-generated method stub

        // TODO Auto-generated method stub
        ViewHolder holder;
        LayoutInflater inflater =  activity.getLayoutInflater();

        if (convertView == null)
        {
            convertView = inflater.inflate(R.layout.listview_row, null);
            holder = new ViewHolder();
            holder.txtSensorName = (TextView) convertView.findViewById(R.id.sensorName);
            holder.txtValue = (TextView) convertView.findViewById(R.id.value);
            holder.txtDate = (TextView) convertView.findViewById(R.id.date);
            holder.imageView = (ImageView) convertView.findViewById(R.id.imageView);
            convertView.setTag(holder);
        }
        else
        {
            holder = (ViewHolder) convertView.getTag();
        }

        HashMap<String, String> map=list.get(position);
        holder.txtSensorName.setText(map.get(LISTVIEW_SENSORNAME));
        holder.txtValue.setText(map.get(LISTVIEW_VALUE));
        holder.txtDate.setText(map.get(LISTVIEW_DATE));

        if(Integer.parseInt(map.get(LISTVIEW_SENSORID))==1){
            holder.imageView.setBackgroundResource(R.drawable.ic_wb_sunny_black_24dp);
        }
        else if (Integer.parseInt(map.get(LISTVIEW_SENSORID))==5){
            holder.imageView.setBackgroundResource(R.drawable.ic_cloud_queue_black_24dp);
        }
        else{
            holder.imageView.setBackgroundResource(R.drawable.ic_dashboard_black_24dp);
        }

        return convertView;
    }

}
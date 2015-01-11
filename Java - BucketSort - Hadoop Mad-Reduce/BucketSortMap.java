package Bucksort;

//import java.io.File;
//import java.io.FileReader;
//import java.io.FileWriter;
//import java.io.IOException;
//import java.io.InputStreamReader;
//import java.io.BufferedReader;
//import java.io.BufferedWriter;
//import java.io.OutputStreamWriter;
import java.util.Arrays;
//import java.util.Collections;
//
import java.io.IOException;


import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

public class BucketSortMap extends Mapper<Object, Text, Text, Text> {
	private final static IntWritable one = new IntWritable(1); // value = 1

	public void map(Object key, Text value, Context context) {
		try {
			
			String[] arr=value.toString().split(" ");
			Arrays.sort(arr);
			String s= Arrays.toString(arr);
			context.write(new Text(s), new Text(one.toString()));
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
}
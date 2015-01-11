package Bucksort;

//
import java.io.IOException;
//
import org.apache.hadoop.io.Text;

import org.apache.hadoop.mapreduce.Reducer;

public class BucketSortReduce extends Reducer<Text, Text, Text, Text> {
	int total = 0;

	public void reduce(Text key, Text values, Context context) throws IOException, InterruptedException {

		context.write(key, values);

	}
}

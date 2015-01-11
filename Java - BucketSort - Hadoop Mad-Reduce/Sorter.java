package Bucksort;

import java.io.IOException;
import java.io.BufferedWriter;
import java.io.OutputStreamWriter;
import java.util.Random;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;

public class Sorter {

	static int size = 100, buckets = 10;

	public int partition(float[] a, int left, int right, int pivot) {
		float pivotValue = a[pivot], temp;
		int index = left;
		int i;

		// move pivot to rightmost of the array
		a[pivot] = a[right];
		a[right] = pivotValue;

		/*
		 * We start from left of the array and keep comparing the values. If we
		 * find any value greater than pivot we swap the pivot with that value.
		 * this is done so that the value on the left are always lesser than
		 * pivot
		 */
		for (i = left; i < right; i++) {
			if (a[i] < pivotValue) {
				temp = a[i];
				a[i] = a[index];
				a[index] = temp;
				index++;
			}
		}
		/*
		 * move pivot back to its initial position
		 */
		temp = a[index];
		a[index] = a[right];
		a[right] = temp;
		return index;

	}

	/*
	 * the main aspect of the function is find the pivot value which is pivot
	 * given and return the value next thing is all the value in the left index
	 * of pivot index is smaller all the values in the right index is greater.
	 */
	public float quickSelect(float[] a, int left, int right, int pivot) {

		// base condition if list contains one element return that
		if (left == right)
			return a[left];

		/*
		 * select the pivot index between left and right i am selecting the mid
		 * value
		 */

		int pivotindex = (right + left) / 2;
		pivotindex = partition(a, left, right, pivotindex);
		// int size_leftarray = pivotposition - low + 1;
		// pivot in its correct position
		if (pivot == pivotindex)
			return a[pivotindex];
		else if (pivot < pivotindex)
			return quickSelect(a, left, pivotindex - 1, pivot);
		else
			return quickSelect(a, pivotindex + 1, right, pivot);
	}

	public float[] randomNumberGeneratorNormal(float[] arr, int problemSize) {

		float maxNum = (float) (problemSize * 10.0);
		float mean = (float) (maxNum / 2.0);
		float var = (float) Math.sqrt(maxNum);
		Random rnd = new Random();

		for (int i = 0; i < problemSize; i++) {
			arr[i] = (float) (mean + rnd.nextGaussian() * var);
			// System.out.println(i + " " + arr[i]);
		}

		return arr;
	}
//write the file to hdfs path using FS and bufferedwriter.
	public void fileWrite(float[] arr, int[] pivot) {
		StringBuffer filePath = new StringBuffer();

		try {

			for (int i = 0; i < buckets; i++) {
				filePath.append("Mapinput/InputFile");
				filePath.append(i);
				filePath.append(".txt");
				Path pt = new Path(filePath.toString());
				FileSystem fs = FileSystem.get(new Configuration());
				BufferedWriter br = new BufferedWriter(new OutputStreamWriter(fs.create(pt, true)));
				// TO append data to a file, use fs.append(Path f)

				StringBuffer line = new StringBuffer();
				for (int j = pivot[i]; j < pivot[i + 1]; j++) {
					line.append(arr[j]);
					line.append(" ");
				}

				// System.out.println(line);
				br.write(line.toString());
				br.flush();
				br.close();
				filePath.setLength(0);
				// System.out.println("write done for " + i + "th file");

			}

		} catch (IOException e) {
			e.printStackTrace();

		}
	}

	// this is the test code

	public static void main(String[] args) {
		size = Integer.valueOf(args[3]);

		// Configuration conf = new Configuration();
		System.out.println("Problem size : " + size + "\tNumber of Buckets : " + buckets);
		Configuration conf = new Configuration();
		float[] arr = new float[size];
		int[] pivots = new int[buckets + 1];
		for (int i = 0; i < buckets; i++) {
			pivots[i] = i * (size / buckets);
		}
		pivots[buckets] = size;
		Sorter S = new Sorter();
		arr = S.randomNumberGeneratorNormal(arr, size);
		System.out.println("done from random gen");
		for (int i = 1; i < buckets; i++) {
			System.out.println(pivots[i] + "th largest valus is");
			float value = S.quickSelect(arr, pivots[i - 1], size - 1, pivots[i]);
			System.out.println(value);

		}
		System.out.println("done spliting");

		// hadoop part starts here.
		try {

			Job job = new Job(conf, "Bucketsort");
			job.setJarByClass(Sorter.class);
			job.setMapperClass(BucketSortMap.class);
			job.setCombinerClass(BucketSortReduce.class);
			job.setReducerClass(BucketSortReduce.class);

			job.setOutputKeyClass(Text.class);
			job.setOutputValueClass(Text.class);

			job.setInputFormatClass(TextInputFormat.class);
			job.setOutputFormatClass(TextOutputFormat.class);

			job.setMapOutputKeyClass(Text.class);
			job.setMapOutputValueClass(Text.class);

			S.fileWrite(arr, pivots);

			FileInputFormat.addInputPath(job, new Path(args[1]));
			FileOutputFormat.setOutputPath(job, new Path(args[2]));
			long startTime = System.currentTimeMillis();

			

			int result = job.waitForCompletion(true) ? 0 : 1;
			System.out.println(result);
			long endTime = System.currentTimeMillis();
			long totalTime = endTime - startTime;
			System.out.println("Running time is : " + (float) (totalTime) / 1000);

		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (ClassNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

	}

}

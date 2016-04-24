import java.util.Scanner;
import java.io.*;
import java.util.ArrayList;

public class Run
{
    public static void main(String[] args)
    {
        //Execuion time, node size, filename, batch of sql statements, connector
        //int size = Integer.parseInt(args[0]);
        int size = 1;
        int threadCount = Integer.parseInt(args[0]);
        long time = 0;
        ArrayList<String> sqlStmt;
        Connector link = new Connector();
        
        //Create connection to client
        link.init(size);
 
        //read input file, run instructions using threads, print results to file
        sqlStmt = readFile("input.txt");
        time = time + link.runProcedure(sqlStmt, threadCount);
        
        System.out.println("Done, results written to execTime.txt");
        String result = printOut(time, threadCount);

        writeFile("execTime.txt", result);
        link.close();
    }

    private static String printOut(long time, int threadCount)
    {
        return "Execution time: " + String.valueOf(time) + " Threads: " + String.valueOf(threadCount) + '\n';
    }

    //file format
    private static ArrayList<String> readFile(String filename)
    {
        try
        {
            Scanner read = new Scanner(new File(filename));
            ArrayList<String> file = new ArrayList<String>();

            while(read.hasNextLine())
            {
                file.add(read.nextLine());
            }
            return file;
        } catch (Exception e)
        {
            e.printStackTrace();
            return null;
        }
    }

    private static void writeFile(String filename, String content)
    {
        File file = null;
        boolean temp;
        
        try
        {
            file = new File(filename);
            temp = file.createNewFile();
            FileWriter writer = new FileWriter(file, true);
            writer.write(content);
            writer.flush();
            writer.close();
        } catch(Exception e)
        {
            e.printStackTrace();
        }

    }

    //useful for AdHoc since it takes in a multiple SQL statements as a String
    private static String arrayToString(ArrayList<String> array)
    {
        String convertArray = "";
        for(int i = 0; i < array.size(); i++)
        {
            convertArray = convertArray.concat(array.get(i));
        }

        return convertArray;
    }
}

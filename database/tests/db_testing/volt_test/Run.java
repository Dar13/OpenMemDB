import java.util.Scanner;
import java.io.*;
import java.util.ArrayList;

public class Run
{

    public final static int NUM_OPERATIONS = 4999;

    public static void main(String[] args)
    {
        //Execuion time, node size, filename, batch of sql statements, connector
        int size = Integer.parseInt(args[0]);
        int threadCount = Integer.parseInt(args[1]);
        String[] filename = {"insert.txt", "select.txt", "drop.txt"};
        ArrayList<String> sqlStmt;
        Connector link = new Connector();
        
        //STORED PROCEDURE IMPLEMENTATION
        //Create connection to client
        link.init(size);

        //mixed.txt contains mixed sql inserts and selects
        long selectTime = 0, insertTime = 0, mixedTime = 0;
        ArrayList<String> sqlBatch = (readFile("mixed.txt"));
        ArrayList<String> insertSqlBatch = (readFile("insert.txt"));

        //insertTime = link.runProcedure("insert", Run.NUM_OPERATIONS, threadCount);
        insertTime = link.runProcedure(insertSqlBatch, threadCount);
        //selectTime = link.runProcedure("select", Run.NUM_OPERATIONS, threadCount);
        mixedTime = mixedTime + link.runProcedure(sqlBatch, threadCount);


        System.out.println("Done, results written to execTime.txt");
        

        String insertResult = printOut(insertTime, threadCount);
        //String selectResult = printOut(selectTime, threadCount);
        String mixedResult = printOut(mixedTime, threadCount);

        writeFile("execTime.txt", "Insert Test: ");
        writeFile("execTime.txt", insertResult);
        //writeFile("execTime.txt", "Select Test:");
        //writeFile("execTime.txt", selectResult);
        writeFile("execTime.txt", "Mixed Test:");
        writeFile("execTime.txt", mixedResult);
        link.close();

        /*
        //initalize client using host name and generate multiple node connections and connect them to host
        link.init(size);

        //check each file for instructions to run
        for(int i = 0; i < filename.length; i++)
        {
            File file = new File(filename[i]);

            //if that file exists then execute it's instructions using AdHoc with thread splitting up work
            if(file.exists() && !file.isDirectory())
            {
                System.out.println("Executing AdHoc SQL Instructions");
                sqlStmt = readFile(filename[i]);

                //Parse SQL statements to make sure they are valid
                for(int j = 0; j < sqlStmt.size(); j++)
                {
                    sqlStmt.set(j, parseSQL(sqlStmt.get(j)));
                }

                if(filename[i] != "create" || filename[i] != "drop.txt")
                    time = time + link.runThread(sqlStmt, threadCount);

                writeFile("execTime.txt", time/1000, threadCount);
            }
        }

        //write to execTime the time it took and the size of cluster and number of threads
        System.out.println("AdHoc Execution Complete\n"+ "Execution Time: "+ time + " Threads: " + threadCount);

        link.close();
        //link.shutdown();
        */

    }

    private static String printOut(long time, int threadCount)
    {
        return "Execution time: " + String.valueOf(time) + " Threads: " + String.valueOf(threadCount) + '\n';
    }

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

    //Some data types are not supported such as STRING or DATE, this must be changed to VARCHAR and TIMESTAMP
    //For select statements only one equal sign allowed
    private static String parseSQL(String input)
    {
        String clean = "STRING";
        String replace = "VARCHAR";

        String clean2 = "DATE";
        String replace2 = "TIMESTAMP";

        String clean3 = "==";
        String replace3 = "=";
        String temp, temp2, parse;
        
        temp = input.replaceAll(clean, replace);
        temp2 = temp.replaceAll(clean3, replace3);
        parse = temp2.replaceAll(clean2, replace2);

        return parse;
    }
}

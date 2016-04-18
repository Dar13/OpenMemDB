import java.util.Scanner;
import java.io.*;
import java.util.ArrayList;

public class main
{
    public static void main(String[] args)
    {
        //Execuion time, node size, filename, batch of sql statements, connector
        long time = 0;
        int size = Integer.parseInt(args[0]);
        int threadCount = Integer.parseInt(args[1]);
        String[] filename = {"create.txt", "insert.txt", "select.txt", "drop.txt"};
        ArrayList<String> sqlStmt;
        connector link = new connector();

        link.initJDBC(size);
        
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

                time = link.runJDBC(sqlStmt);
                writeFile("execTime.txt", time/1000, threadCount);
            }
        }

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

                time = link.runThread(sqlStmt, threadCount);
                writeFile("execTime.txt", time/1000, threadCount);
            }
        }

        //write to execTime the time it took and the size of cluster and number of threads
        System.out.println("AdHoc Execution Complete\n"+ "Execution Time: "+ time + " Threads: " + threadCount);

        link.close();
        //link.shutdown();
        
        
         * STORED PROCEDURE IMPLEMENTATION
        int nodes = Integer.parseInt(args[0]);
        int threadCount = nodes;
        connector link = new connector();
        link.init(nodes);

        //Load db and select from db
        int numInserts=3;
        long time = 0;
        long time2 = 0;
        for(int i = 0; i < numInserts; i++)
        {
            time = link.runProcedure("insert", numInserts, threadCount);
            time2 = link.runProcedure("select", numInserts, threadCount);
        }

        writeFile("execTime.txt", time, threadCount);
        writeFile("execTime.txt", time2, threadCount);
        */
    }

    private static ArrayList<String> readFile(String filename)
    {
        try
        {
            Scanner read = new Scanner(new File(filename));
            ArrayList<String> file = new ArrayList<String>();
            int i = 0;

            while(read.hasNextLine())
            {
                file.add(i, read.nextLine());
                i++;
            }
            return file;
        } catch (Exception e)
        {
            e.printStackTrace();
            return null;
        }
    }

    private static void writeFile(String filename, long time, int node)
    {
        File file = null;
        boolean temp;
        try
        {
            file = new File(filename);
            temp = file.createNewFile();
            FileWriter writer = new FileWriter(file, true);
            String content = "Execution time: " + String.valueOf(time) + " " + "Threads: " + String.valueOf(node) + '\n';
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

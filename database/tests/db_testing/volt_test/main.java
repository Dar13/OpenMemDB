import java.util.Scanner;
import java.io.*;
import java.util.ArrayList;

public class main
{
    public static void main(String[] args)
    {

        long time = 0;
        int size = Integer.parseInt(args[0]);
        String[] filename = {"create.txt", "insert.txt", "select.txt", "drop.txt"};
        ArrayList<String> sqlStmt;
        connector link = new connector();

        //initalize client using host name and generate multiple node connections and connect them to host
        link.init(size);

        String batch = "";

        //run single batch sql instructions
        for(int i = 0; i < filename.length; i++)
        {
            File file = new File(filename[i]);
            if(file.exists() && !file.isDirectory())
            {
                sqlStmt = readFile(filename[i]);

                System.out.println(sqlStmt.size());
                //Parse SQL statements to make sure they are valid
                for(int j = 0; j < sqlStmt.size(); j++)
                {
                    sqlStmt.set(j, parseSQL(sqlStmt.get(j)));
                    System.out.println("New statement: "+ sqlStmt.get(j));
                }

                batch = arrayToString(sqlStmt);
                time = link.run(batch);
                writeFile("execTime.txt", time, size);
            }
        }

        link.close();
        link.shutdown();
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
            String content = "Execution time: " + String.valueOf(time) + " " + "Cluster nodes: " + String.valueOf(node) + '\n';
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

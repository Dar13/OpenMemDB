import java.util.Scanner;
import java.io.*;
import java.util.ArrayList;

public class main
{
    public static void main(String[] args)
    {

        long time = 0;
        int size = Integer.parseInt(args[0]);
        String filename = "output.txt";
        ArrayList<String> sqlStmt = readFile(filename);
        connector link = new connector();

        //Parse SQL statements to make sure they are valid
        for(int i = 0; i < sqlStmt.size(); i++)
        {
            sqlStmt.set(i, parseSQL(sqlStmt.get(i)));
        }

        //initalize client using host name and generate multiple node connections and connect them to host
        link.init(size);

        //run single batch sql instructions
        String batch = arrayToString(sqlStmt);
        time = link.run(batch);
        writeFile(time, size);

        link.close();
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

    private static void writeFile(long time, int node)
    {
        try
        {
            PrintWriter writer = new PrintWriter("execTime.txt", "UTF-8");
            writer.println(output + " " + node);
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
    private static String parseSQL(String input)
    {
        String clean = "STRING";
        String replace = "VARCHAR";

        String clean2 = "DATE";
        String replace2 = "TIMESTAMP";
        String temp, parse;
        
        temp = input.replaceAll(clean, replace);
        parse = temp.replaceAll(clean2, replace2);

        return parse;
    }
}

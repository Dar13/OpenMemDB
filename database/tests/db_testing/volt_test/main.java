import java.util.Scanner;
import java.io.*;

public class main
{
    public static void main(String[] args)
    {
        long time = 0;
        //int threadCount;

        int size = 2;
        String filename = "output.txt";
        String sqlStmt = parseFile(readFile(filename));
        connector link = new connector();

        //initalize client using host name and generate multiple node connections and connect them to host
        link.init(size);

        time = link.run(sqlStmt);
        //threadCount = 4;
        //time = link.insert(threadCount);
        System.out.println("Execution Time: " + time);
        //System.out.println("Threads : " + threadCount);

        //link.print();
        link.close();
    }

    private static String readFile(String filename)
    {
        try
        {
            Scanner read = new Scanner(new File(filename));
            String file = "";

            while(read.hasNextLine())
            {
                file = file.concat(read.nextLine());
            }
            return file;
        } catch (Exception e)
        {
            e.printStackTrace();
            return null;
        }
    }

    //Some data types are not supported such as STRING or DATE, this must be changed to VARCHAR and TIMESTAMP
    private static String parseFile(String input)
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

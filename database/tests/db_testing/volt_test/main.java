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
        String sqlStmt = readFile(filename);
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
}

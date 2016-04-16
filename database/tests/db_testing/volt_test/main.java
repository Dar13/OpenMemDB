
public class main
{
    public static void main(String[] args)
    {
        //long time;
        //int threadCount;

        int size = 2;
        connector link = new connector();

        //initalize client using host name and generate multiple node connections and connect them to host
        //default node size is 64
        link.init(size);
        
        link.run();

        //threadCount = 4;
        //time = link.insert(threadCount);
        //System.out.println("Execution Time: " + time);
        //System.out.println("Threads : " + threadCount);
    
        //link.print();
        link.close();
    }

    public String readFile()
    {

    }
}

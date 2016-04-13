
public class main
{
    public static void main(String[] args)
    {
        String host = "localhost";
        //long time;
        //int threadCount;

        connector link = new connector(host);

        //initalize client using host name and generate multiple node connections and connect them to host
        //default node size is 64
        link.init();
        
        //threadCount = 4;
        //time = link.insert(threadCount);
        //System.out.println("Execution Time: " + time);
        //System.out.println("Threads : " + threadCount);
    
        //link.print();
        link.close();
    }
}

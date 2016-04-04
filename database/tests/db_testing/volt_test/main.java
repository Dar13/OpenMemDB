
public class main
{
    public static void main(String[] args)
    {
        String name = "localhost";
        long time;
        int threadCount;

        connector link = new connector(name);

        link.init();
        threadCount = 4;
        time = link.insert(threadCount);
        System.out.println("Execution Time: " + time);
        System.out.println("Threads : " + threadCount);
    
        //link.print();
        link.close();
    }
}

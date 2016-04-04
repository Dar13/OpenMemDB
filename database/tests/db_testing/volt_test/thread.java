public class thread extends Thread
{
    private org.voltdb.client.Client myApp;
    private int data;

    public thread(org.voltdb.client.Client myApp, int data)
    {
        this.data = data;
        this.myApp = myApp;
    }

    public void insert()
    {
        //length of data set
        int size = 1;

        for(int i = 0; i < size; i++)
        {
            try {
                System.out.println("Inserting data... " +data);
                myApp.callProcedure("Insert", data);
            } catch(Exception e)
            {
                e.printStackTrace();
            }
        }
    }

    public void start(char flag)
    {
        switch(flag)
        {
            //run insert method
            case 'i':
                {
                    insert();
                    break;
                }
        }
    }
}


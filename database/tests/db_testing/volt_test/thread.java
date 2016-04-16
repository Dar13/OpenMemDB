import java.util.ArrayList;

public class thread extends Thread
{
    private org.voltdb.client.Client myApp;
    private ArrayList<String> batch;

    public thread(org.voltdb.client.Client myApp, ArrayList<String> batch)
    {
        this.myApp = myApp;
        this.batch = batch;
    }

    public void insert()
    {
        //Calls procedure using client and uses stored procedure to do so
        for(int i = 0; i < batch.size(); i++)
        {
            try {
                //myApp.callProcedure("create");
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


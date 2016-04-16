import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.List;
import java.util.Properties;
import java.util.concurrent.BrokenBarrierException;
import java.util.concurrent.CyclicBarrier;

/**
 * Created by mike on 4/11/16.
 */
public class Worker implements Runnable {
    private final int numThreads;
    private final long numOps;
    private final List<String> commands;
    private final int threadNum;
    private final CyclicBarrier gate;

    public Worker(int numThreads, long numOps, List<String> commands, int threadNum, CyclicBarrier gate) {
        this.numThreads = numThreads;
        this.numOps = numOps;
        this.commands = commands;
        this.threadNum = threadNum;
        this.gate = gate;
    }


    @Override
    public void run() {

        Properties properties = new Properties();
        properties.put("user", Constants.USER);
        properties.put("password", Constants.PASSWORD);
        try (Connection conn = DriverManager.getConnection(Constants.CONNECTION, properties)) {
            executeSQL(conn, "USE test");
            int startIndex = (commands.size() / numThreads) * threadNum;
            int totOps = (commands.size()/numThreads);

            // Wait for the gate that is controlled by the main thread before running
            try {
                this.gate.await();
            } catch (InterruptedException e) {
                e.printStackTrace();
            } catch (BrokenBarrierException e) {
                e.printStackTrace();
            }

            for (int i = 0; i < totOps; ++i) {
                executeSQL(conn, commands.get(i+startIndex));
            }
        } catch (SQLException e) {
            e.printStackTrace();
        }

    }

    public static void executeSQL(Connection conn, String sql) throws SQLException {
        try (Statement stmt = conn.createStatement()) {
            //System.out.println("Attempting to execute statement: "+sql);
            stmt.execute(sql);
        } catch (SQLException e) {
            e.printStackTrace();
            System.err.println("Trying to execute statement: "+sql);
            System.exit(1);
        }
    }
}

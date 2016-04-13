/**
 * Created by mike on 4/11/16.
 */
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.sql.*;
import java.util.ArrayList;
import java.util.List;
import java.util.Properties;
import java.util.concurrent.*;

public class Sample {

    private enum TestType {
        CREATE,
        INSERT,
        SELECT
    }

    private static void ResetEnvironment() throws SQLException {
        Properties p = new Properties();
        p.put("user", Constants.USER);
        p.put("password", Constants.PASSWORD);
        try (Connection conn = DriverManager.getConnection(Constants.CONNECTION, p)) {
            for (String query: new String[] {
                    "DROP DATABASE IF EXISTS test",
                    "CREATE DATABASE test",
                    "USE test"
            }) {
                Worker.executeSQL(conn, query);
            }
        }
    }

    public static void main(String[] args) throws ClassNotFoundException, SQLException, InterruptedException {
        Class.forName(Constants.dbClassName);

        if (args.length < 3) {
            System.out.println("Please provide arguments\n<num threads> <num operations> <path to sql_statement file>");
            return;
        }
        int numThreads = Integer.valueOf(args[0]);
        long numOps = Long.valueOf(args[1]);
        String path = args[2];

        TestType testType = null;

        List<String> allCommands = null;
        try {
            // TODO: Change file path to command line arg
            allCommands = Files.readAllLines(Paths.get(path));
        } catch (IOException e) {
            e.printStackTrace();
        }

        if (allCommands == null || allCommands.isEmpty()) {
            System.err.println("Commands not properly read");
        }

        // The first line in all commands should be the name of the test
        String testName = allCommands.get(0);

        // Holds all of the parsed command lists, is immutable
        Commands commands = new Commands(allCommands);

        Properties properties = new Properties();
        properties.put("user", Constants.USER);
        properties.put("password", Constants.PASSWORD);

        // Need to set up database for testing
        ResetEnvironment();

        if (testName.equalsIgnoreCase("create_test")) {
            // Just need to execute commands in the order they are presented
            // and time all commands
            testType = TestType.CREATE;
        } else if (testName.equalsIgnoreCase("insert_test")) {
            // Need to run create statements then time the insert statements
            try (Connection conn = DriverManager.getConnection(Constants.CONNECTION, properties)) {
                Worker.executeSQL(conn, "USE test");
                for (String createCommand: commands.getCreateCommands()) {
                    Worker.executeSQL(conn, createCommand);
                }
            } catch (SQLException e) {
                e.printStackTrace();
                return;
            }
            testType = TestType.INSERT;

        } else if (testName.equalsIgnoreCase("select_test")) {
            // Need to run create statements then insert statements then time select statements
            try (Connection conn = DriverManager.getConnection(Constants.CONNECTION, properties)) {
                Worker.executeSQL(conn, "USE test");
                for (String createCommand: commands.getCreateCommands()) {
                    Worker.executeSQL(conn, createCommand);
                }
                for (String insertCommand: commands.getInsertCommands()) {
                    Worker.executeSQL(conn, insertCommand);
                }
            } catch (SQLException e) {
                e.printStackTrace();
                return;
            }
            testType = TestType.SELECT;

        } else {
            System.err.println("Test name is not of expected type: "+testName);
        }

        // Need a barrier to make sure that all threads start at the same time
        final CyclicBarrier gate = new CyclicBarrier(numThreads+1);
        ArrayList<Thread> threads = new ArrayList<>();
        for (int i = 0; i < numThreads; i++) {
            List<String> finalAllCommands;
            if (testType == null) {
                System.err.println("Test Type not set properly: null");
                return;
            }
            switch (testType) {
                case CREATE:
                    finalAllCommands = commands.getCreateCommands();
                    break;
                case INSERT:
                    finalAllCommands = commands.getInsertCommands();
                    break;
                case SELECT:
                    finalAllCommands = commands.getSelectCommands();
                    break;
                default:
                    System.err.println("Test type not properly set");
                    return;
            }
            Runnable thread = new Worker(numThreads, numOps, finalAllCommands, i, gate);
            threads.add(new Thread(thread));
        }

        // Pseudo start the threads
        // Should not actually run here, should wait for main to call await
        threads.forEach(Thread::start);
        long starTime;
        try {
            starTime = System.nanoTime();
            gate.await();
        } catch (BrokenBarrierException e) {
            e.printStackTrace();
            return;
        }

        // Join the threads and check the time
        for (Thread t: threads) {
            t.join();
        }

        long totalTime = System.nanoTime() - starTime;

        System.out.println(testName+" running time (Micro): "+totalTime/1000);

    } // End of main
}

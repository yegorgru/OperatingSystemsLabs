import java.io.*;
import java.util.logging.Logger;

public class MemoryManagement
{
    private static final Logger log = Logger.getLogger(MemoryManagement.class.getName());

    public static void main(String[] args)
    {
        if (args.length != 2) {
            log.severe("Usage: 'java MemoryManagement <COMMAND FILE> <PROPERTIES FILE>'");
            System.exit(-1);
        }
        checkFile(args[0]);
        checkFile(args[1]);
        Application application = new Application( "Memory Management");
        application.init(args[0], args[1]);
    }

    public static void checkFile(String path) {
        File file = new File(path);
        if (!(file.exists())) {
            log.severe("MemoryM: error, file '" + file.getName() + "' does not exist." );
            System.exit( -1 );
        }
        if (!(file.canRead())) {
            log.severe("MemoryM: error, read of " + file.getName() + " failed.");
            System.exit( -1 );
        }
    }
}

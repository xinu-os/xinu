Xinu Helper Class
=================

Here is the full Java source code for the *Xinu.java* helper class.

::

    import java.util.Scanner;
    public class Xinu
    {
        public static int readint()
        {
            Scanner scanner = new Scanner(System.in);
            return scanner.nextInt();
        }
        public static void printint(int x)
        {
            System.out.println(x);
        }
        public static void print(String s)
        {
            System.out.print(s);
        }
        public static void println()
        {
            System.out.println();
        }
        public static void yield()
        {
            Thread thisThread = Thread.currentThread();
            try
            {
                thisThread.yield();
            }
            catch (Exception e)
            {
                System.out.println(e);
            }
        }
        public static void sleep(int time)
        {
            Thread thisThread = Thread.currentThread();
            try
            {
                thisThread.sleep(time);
            }
            catch (Exception e)
            {
                System.out.println(e);
            }
        }
        public static void threadCreate(Thread t)
        {
            t.start();
        }
    }


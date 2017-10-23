package philisopers;

/**
 * Created by Sano on 19.10.2017.
 */
public class ForkImpl implements Fork{
     volatile boolean state =false;

    @Override
    public boolean state() {
        synchronized (this) {
            return state;
        }
    }

    public boolean getState() {
        return state;
    }

    public void setState(boolean state) {
        this.state = state;
    }
}

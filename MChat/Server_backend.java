package coms319;

import java.awt.Dimension;
import java.awt.EventQueue;
import java.awt.FlowLayout;
import java.awt.Font;
import java.awt.ScrollPane;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.image.BufferedImage;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.NoSuchElementException;
import java.util.Scanner;

import javax.imageio.ImageIO;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextField;

public class Server_backend {

	public static void main(String args[]) throws IOException {
		
		ArrayList<Socket> ConnectionList = new ArrayList<>(10);
		ArrayList<String> UserList =  new ArrayList<>(10);
		ServerSocket sSocket = null;
		int cnt=0;
		
		try{
		sSocket = new ServerSocket(4200);
		System.out.println(sSocket);
		}catch(IOException e){
			
			System.out.println("Socket Failure : Lister");
			System.exit(-1);
		}
				
		

		File txtFile = new File("./chatLog.txt");
		FileWriter fWrite = new FileWriter(txtFile,true);
		
		ChatTxtLog cTxtLog = new ChatTxtLog(fWrite);
		ChatLog cLog = new ChatLog();
		
		DateFormat dateFormat = new SimpleDateFormat("yyyy/MM/dd HH:mm:ss");
		Date date = new Date();
		cTxtLog.writeToFile("Server Started :: "+ dateFormat.format(date) + "\n");
		
		
		
		EventQueue.invokeLater(new Runnable() {
			public void run() {
				try {
					
					JFrame frame = new server_chat(cLog,ConnectionList,UserList);
					frame.setTitle("M-Chat Server Admin Console");
					frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
					frame.setBounds(500, 200, 450, 500);
					frame.setVisible(true);
	
				} catch (Exception e) {
					System.out.println("Failed to initialize client connection");
					//e.printStackTrace();
				}
			}
		});
		
		
		while(true){
			
			System.out.println("Connecting...");
			Socket connectionSocket = null;
			try{
				
				connectionSocket = sSocket.accept();
				cnt++;
				ConnectionList.add(connectionSocket);
				System.out.println("A User Has Connected : Client Count = "+cnt);
				cTxtLog.writeToFile("A New Client Has Connected");
				
				Thread cthread = new Thread(new ConnectionManager(connectionSocket,cnt,ConnectionList, UserList,cLog, cTxtLog));
				cthread.start();
				
				
				
			}catch(IOException e){
				
				System.out.println("Connection Error : Accept Failure");
				System.exit(-1);
				
			}	
		}
	}
}
/**
 * Create the frame.
 * @throws IOException 
 * @throws UnknownHostException 
 */
class server_chat extends JFrame {
	
	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;

	public server_chat(ChatLog cLog, ArrayList<Socket> connectionList, ArrayList<String> userList) {
	
		
		JPanel basePanel = new JPanel();
		
		JPanel tPane = new JPanel();
		tPane.setLayout(new BoxLayout (tPane, BoxLayout.Y_AXIS));
	
		JLabel stuff2 = new JLabel("Message:");
		stuff2.setFont(new Font("Arial",Font.PLAIN,12));
		
		JTextField message = new JTextField(null);
		
		JButton btn1 = new JButton("Send");
		btn1.addActionListener(new ActionListener() {
			
			@Override
			public void actionPerformed(ActionEvent e) {

					String msg = message.getText();
					
					try {
						cLog.setMsg("[Server]< "+msg);
						cLog.printToClient(connectionList,0,"");
						message.setText("");
					} catch (IOException e1) {
					
						e1.printStackTrace();
					}
	
			}
		});
		
		JPanel sPanel = new JPanel();
		JPanel nPanel = new JPanel();
		
		nPanel.setMaximumSize(new Dimension(300,25));		
		message.setPreferredSize( new Dimension( 200, 24 ) );
		nPanel.add(message);
		
		sPanel.setLayout(new FlowLayout());
		
		sPanel.add(stuff2);
		sPanel.add(nPanel);
		sPanel.add(btn1);
	
		tPane.add(sPanel);
	    tPane.add(Box.createRigidArea(new Dimension (0, 10)));
	    
	    JPanel pPane = new JPanel();
	    JButton pbutton = new JButton("User Log");
	    JLabel users = new JLabel();
	    
	    pbutton.addActionListener(new ActionListener() {
			
			@Override
			public void actionPerformed(ActionEvent e) {

				String str = "";
				
				for(String s: userList){
					
					str = str+s+" \n";
					
				}
				
				users.setText(str);
				users.repaint();
			}
		});
	    
	    JScrollPane uScroller = new JScrollPane(users);
	    uScroller.setPreferredSize(new Dimension(150,200));
	    
	    pPane.setLayout(new BoxLayout(pPane,BoxLayout.Y_AXIS));
	    pPane.add(pbutton);
	    pPane.add(Box.createRigidArea(new Dimension (0, 10)));
	    pPane.add(uScroller);
	    
		basePanel.add(tPane);
		basePanel.add(pPane);
		this.setContentPane(basePanel);
	}	
	
}



final class ChatTxtLog{

	FileWriter fWriter;
	
	public ChatTxtLog(FileWriter fileWrite){
		
			fWriter = fileWrite;
	}
	
	public void writeToFile(String msg){
		
		try {
			fWriter.write(msg+"\n");
			fWriter.flush();
		} catch (IOException e) {
			System.out.println("Failed to write msg to file");
			//e.printStackTrace();
		}
		
	}
	
	public void closeWriter(){
		
		try {
			fWriter.close();
		} catch (IOException e) {
			System.out.println("Failed to close file writer");
			//e.printStackTrace();
		}
		
	}

}


class ChatLog {
	
	boolean flag = false;
	String msg;
	BufferedImage Img = null;

	
	public ChatLog(){
		this.msg = "";
	}
	
	public void setMsg(String msg){
		
		this.msg = msg;
	}
	
	public void setImg(BufferedImage arrImg) {
		
		Img = arrImg;
		
	}

	public synchronized void printToClient(ArrayList<Socket> conList,int flag,String name) throws IOException{	
		
		for(Socket s:conList){
		
				if(flag == 0){
			
					PrintWriter oStream = new PrintWriter(s.getOutputStream());
					
					oStream.println(flag);
					oStream.flush();
					oStream.println(msg);
					oStream.flush();
				}else if(flag == 1){
					
					OutputStream o = s.getOutputStream();
					PrintWriter oStream = new PrintWriter(o);
					
					oStream.println(flag);
					oStream.flush();
					
					oStream.println(name);
					oStream.flush();
					
					ByteArrayOutputStream byteArrayOutputStream = new ByteArrayOutputStream();
				    ImageIO.write(Img, "jpg", byteArrayOutputStream);

				    byte[] size = ByteBuffer.allocate(4).putInt(byteArrayOutputStream.size()).array();
				    o.write(size);
				    o.flush();
				    o.write(byteArrayOutputStream.toByteArray());
				    o.flush();
					
				}
						
		}
	}
	
}



class ConnectionManager implements Runnable{
	
	ArrayList<Socket> conList;
	ArrayList<String> userList;
	ChatLog cLog;
	Socket Sock;
	int i;
	ChatTxtLog cTxtLog;
	String conUsrName;

	public ConnectionManager(Socket Sock,int i, ArrayList<Socket> connectionList, ArrayList<String> userList, ChatLog cLog, ChatTxtLog cTxtLog) {
		this.cLog = cLog;
		this.Sock = Sock;
		this.i = i;
		this.conList = connectionList;
		this.userList = userList;
		this.cTxtLog = cTxtLog;
		this.conUsrName = "";
		
	}
	
	
	@Override
	public void run() {
		
		
		System.out.println("connected : "+Sock.getInetAddress());
		System.out.println("On Port : "+Sock.getPort());
		
		try {
			//input stream and scanner
			
			InputStream inStream = Sock.getInputStream();
			Scanner Receive =  new Scanner(inStream);
			
			//first line of new connection must be the user name
			
			String uName =  Receive.nextLine(); 	
			
			this.conUsrName = uName;
			
			this.userList.add(uName);
			
			System.out.println("User :: "+ uName);
			
			//loops and holds on nextline for input from clients
			while(true){
			
				String flag = Receive.nextLine();
				System.out.println("Flag :: "+flag);
				
				// when flag is one a msg is being received if 2 a picture is received
				
				if(flag.equals("1")==true){
				
				String rMessage = Receive.nextLine(); 	// gets message
				System.out.println("Message recived : "+ rMessage);
				
				String msg = "["+uName+"]< "+rMessage; 	// builds msg for chat log
				
				cLog.setMsg(msg);						//sets chat log msg		
				cLog.printToClient(conList,0,"");			//broadcast method
				cTxtLog.writeToFile(msg);				//log file
				}else if(flag.equals("2")==true){
					
					try {
						
						
						
						/*
						*sets a byte array to receive data input
						*note: must be large in case of overrun from sync issues
						*large size allows for a read that may be an image or
						*large text left unbuffered in the stream. error handling then
						*corrects the syncing issues
						*/
						byte[] arrSize = new byte[10000];

						inStream.read(arrSize, 0, arrSize.length);				
						int s = ByteBuffer.wrap(arrSize).asIntBuffer().get();

						System.out.println("byte size :: " + s);
						
						if(s==0){
							throw new Exception();
						}
						
						byte[] arrImg = new byte[s];
						inStream.read(arrImg, 0, arrImg.length);

						BufferedImage recvImg = ImageIO.read(new ByteArrayInputStream(arrImg));

						if(recvImg!=null){
							cLog.setImg(recvImg);
							cLog.printToClient(conList, 1,uName);
	
							System.out.println("Received " + recvImg.getHeight() + "x" + recvImg.getWidth() + ": "
									+ System.currentTimeMillis());
						}
					} catch (NegativeArraySizeException e) {
						System.out.println("negitive buff array");
						return;
					} catch (OutOfMemoryError s) {
						System.out.println("stack error");
						return;
					} catch (NullPointerException f) {
						System.out.println("Null Ptr On");
						dumpStream(inStream);
					} catch (Exception e) {
						System.out.println("zero buffer");
					}

				} else {
					//clears the stream in-case of bad transfer
					
					dumpStream(inStream);
					
				}
				
			}
			
		} catch (IOException e) {
		
			e.printStackTrace();
			
		} catch (NoSuchElementException s){
			
			System.out.println("Client Closed");
			cTxtLog.writeToFile("A Client Has Disconnected");
			
		}
	}
	private void dumpStream(InputStream inStream) throws IOException{
		
		inStream.skip(inStream.available());
		
		
	}
	
}

const express = require('express');
const bodyParser = require('body-parser');
const cors = require('cors');
const fs = require('fs');
const path = require('path');
const { SerialPort, ReadlineParser } = require('serialport');
const { WebSocketServer } = require('ws');

const app = express();
const PORT = 5000;

// 아두이노1 (LED 및 WebSocket 처리)
const arduinoPort1 = new SerialPort({
  path: '/dev/ttyACM0',
  baudRate: 9600,
});

// 아두이노2 (키패드 입력 처리)
const arduinoPort2 = new SerialPort({
  path: '/dev/ttyACM1',
  baudRate: 9600,
});

const parser1 = arduinoPort1.pipe(new ReadlineParser({ delimiter: '\n' }));
const parser2 = arduinoPort2.pipe(new ReadlineParser({ delimiter: '\n' }));

app.use(cors());
app.use(bodyParser.json());
app.use(express.static('public'));

// WebSocket 서버 설정
const wss = new WebSocketServer({ port: 8080 });

// WebSocket 연결 처리
wss.on('connection', (ws) => {
  console.log('WebSocket client connected');

  parser1.on('data', (data) => {
    const message = data.trim();
    console.log('Received from Arduino1:', message);
    if (message === 'Change to quiz_index.html') {
      ws.send('REDIRECT_TO_QUIZ_INDEX');
    }
  });

  ws.on('close', () => {
    console.log('WebSocket client disconnected');
  });
});

let joystickCommand = null;
parser2.on('data', (data) => {
  joystickCommand = data.trim();
  console.log('Received from Arduino2:', joystickCommand);
});

app.get('/joystick', (req, res) => {
  res.json({ command: joystickCommand });
  setTimeout(() => {
    joystickCommand = null;
  }, 100); // 초기화
});

app.get('/get-questions', (req, res) => {
  const filePath = path.join(__dirname, 'questions.json');
  const count = parseInt(req.query.count, 10);

  fs.readFile(filePath, 'utf8', (err, data) => {
    if (err) {
      console.error('Error reading questions file:', err);
      res.status(500).json({ error: '문제를 불러오는 데 실패했습니다.' });
      return;
    }

    try {
      const questions = JSON.parse(data);
      const selectedQuestions = questions.slice(0, count); // 요청한 문제 수만큼 반환
      res.json(selectedQuestions);
    } catch (parseError) {
      console.error('Error parsing questions file:', parseError);
      res.status(500).json({ error: '문제 파일 형식이 잘못되었습니다.' });
    }
  });
});

app.post('/control-led', (req, res) => {
  const { correct } = req.body;
  const command = correct ? 'LED1_ON' : 'LED2_ON';

  console.log(`Received LED control request: ${command}`); // 디버깅용 로그

  arduinoPort1.write(`${command}\n`, (err) => {
      if (err) {
          console.error('Error writing to Arduino1:', err);
          res.status(500).send('LED 제어 실패');
      } else {
          console.log(`LED 제어 성공: ${command}`);
          res.send(`LED 제어 성공: ${command}`);
      }
  });
});

app.get('/', (req, res) => {
  res.sendFile(__dirname + '/public/index.html');
});

app.get('/quiz_index', (req, res) => {
  res.sendFile(__dirname + '/public/quiz_index.html');
});

app.listen(PORT, () => {
  console.log(`Server running at http://localhost:${PORT}`);
  console.log(`WebSocket server running at ws://localhost:8080`);
});

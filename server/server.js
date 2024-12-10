const express = require('express');
const bodyParser = require('body-parser');
const cors = require('cors');
const fs = require('fs');
const path = require('path');

const app = express();
const PORT = 3000;

app.use(cors());
app.use(bodyParser.json());

// 문제 제공 엔드포인트
app.get('/get-questions', (req, res) => {
    const filePath = path.join(__dirname, 'questions.json');
    const count = parseInt(req.query.count, 10); // 요청받은 문제 수

    fs.readFile(filePath, 'utf8', (err, data) => {
        if (err) {
            console.error('Error reading questions file:', err);
            res.status(500).json({ error: '문제를 불러오는 데 실패했습니다.' });
            return;
        }

        try {
            const questions = JSON.parse(data);

            // 요청한 개수만큼 문제를 랜덤으로 선택
            const selectedQuestions = questions
                .sort(() => Math.random() - 0.5) // 무작위로 섞기
                .slice(0, count); // 요청된 개수만 자르기

            res.json(selectedQuestions);
        } catch (parseError) {
            console.error('Error parsing questions file:', parseError);
            res.status(500).json({ error: '문제 파일 형식이 잘못되었습니다.' });
        }
    });
});



// 서버 시작
app.listen(PORT, () => {
    console.log(`Server is running on http://localhost:${PORT}`);
});

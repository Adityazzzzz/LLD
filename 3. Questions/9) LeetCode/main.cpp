#include<iostream>
using namespace std;

enum class Language{ CPP,PYTHON,JAVA };
enum class Verdict{ ACCEPTED,WRONG_ANSWER,TIME_LIMIT_EXCEEDED,RUNTIME_ERROR };

struct TestCase{
    string input;
    string expectedOutput;
};

struct ExecutionResult{
    Verdict verdict;
    string actualOutput;
    double executionTimeMs;
};

class Submission{
public:
    string submissionId;
    string problemId;
    string userId;
    Language language;
    string code;
    Verdict status;

    Submission(string sId,string pId,string uId,Language lang,string c) : submissionId(sId),problemId(pId),userId(uId),language(lang),code(c),status(Verdict::WRONG_ANSWER){}
};

class Problem{
public:
    string problemId;
    string title;
    vector<TestCase> testCases;
    double timeLimitMs;

    Problem(string id,string t,double limit):problemId(id),title(t),timeLimitMs(limit){}

    void addTestCase(string input,string expected){
        testCases.push_back({input,expected});
    }
};

class ExecutionStrategy{
public:
    virtual ~ExecutionStrategy() = default;
    virtual ExecutionResult execute(const string& code,const TestCase& testCase,double timeLimit) = 0;
};

class CppExecutionStrategy:public ExecutionStrategy{
public:
    ExecutionResult execute(const string& code,const TestCase& testCase,double timeLimit) override{
        if(testCase.input == "[-1,2,3]"){
            return{Verdict::TIME_LIMIT_EXCEEDED,"",timeLimit + 10.0};
        }
        
        string output = (testCase.input == "[2,7,11,15],9") ? "[0,1]":"error";
        return{Verdict::ACCEPTED,output,12.5};
    }
};

class PythonExecutionStrategy:public ExecutionStrategy{
public:
    ExecutionResult execute(const string& code,const TestCase& testCase,double timeLimit) override{
        string output = (testCase.input == "[2,7,11,15],9") ? "[0,1]":"error";
        return{Verdict::ACCEPTED,output,45.0};
    }
};

class ExecutionStrategyFactory{
public:
    static ExecutionStrategy* getStrategy(Language lang){
        switch(lang){
            case Language::CPP: return new CppExecutionStrategy();
            case Language::PYTHON: return new PythonExecutionStrategy();
            default: return nullptr;
        }
    }
};

class LeetCodeJudgeSystem{
private:
    unordered_map<string,Problem*> problems;
    unordered_map<string,Submission*> submissions;
    int submissionCounter = 1;
public:
    ~LeetCodeJudgeSystem(){
        for(auto& pair:problems) delete pair.second;
        for(auto& pair:submissions) delete pair.second;
    }

    void addProblem(Problem* p){
        problems[p->problemId] = p;
    }

    Submission* submitSolution(string userId,string problemId,Language lang,string code){
        if(problems.find(problemId) == problems.end()){
            cout << "[ERROR] Problem ID not found.\n";
            return nullptr;
        }

        string sId = "SUB-" + to_string(submissionCounter++);
        Submission* sub = new Submission(sId,problemId,userId,lang,code);
        submissions[sId] = sub;

        judge(sub);
        return sub;
    }

    void judge(Submission* submission){
        Problem* problem = problems[submission->problemId];
        ExecutionStrategy* strategy = ExecutionStrategyFactory::getStrategy(submission->language);

        if(!strategy){
            cout << "[ERROR] Unsupported execution language.\n";
            return;
        }

        Verdict finalVerdict = Verdict::ACCEPTED;

        for(size_t i = 0; i < problem->testCases.size(); ++i){
            const auto& tc = problem->testCases[i];
            ExecutionResult res = strategy->execute(submission->code,tc,problem->timeLimitMs);

            if(res.executionTimeMs > problem->timeLimitMs){
                finalVerdict = Verdict::TIME_LIMIT_EXCEEDED;
                break;
            }
            if(res.actualOutput != tc.expectedOutput){
                finalVerdict = Verdict::WRONG_ANSWER;
                break;
            }
        }

        submission->status = finalVerdict;
        delete strategy;

        printVerdict(submission);
    }
private:
    void printVerdict(Submission* sub){
        cout << "Submission " << sub->submissionId << " Result: ";
        switch(sub->status){
            case Verdict::ACCEPTED: cout << "ACCEPTED\n"; break;
            case Verdict::WRONG_ANSWER: cout << "WRONG ANSWER\n"; break;
            case Verdict::TIME_LIMIT_EXCEEDED: cout << "TIME LIMIT EXCEEDED\n"; break;
            case Verdict::RUNTIME_ERROR: cout << "RUNTIME ERROR\n"; break;
        }
    }
};

int main(){
    LeetCodeJudgeSystem judgeSystem;

    //Setup Problem & Test cases
    Problem* twoSum = new Problem("P-1","Two Sum",50.0);
    twoSum->addTestCase("[2,7,11,15],9","[0,1]");
    judgeSystem.addProblem(twoSum);

    //Submit C++ Correct Solution
    judgeSystem.submitSolution("UserA","P-1",Language::CPP,"class Solution{ ... }");

    //Submit Python Solution
    judgeSystem.submitSolution("UserB","P-1",Language::PYTHON,"def twoSum(nums,target): ...");

    //Submit Timeout Case
    Problem* timeoutProb = new Problem("P-2","Graph Cycles",20.0);
    timeoutProb->addTestCase("[-1,2,3]","Cycle Found");
    judgeSystem.addProblem(timeoutProb);

    judgeSystem.submitSolution("UserC","P-2",Language::CPP,"while(true){}");

    return 0;
}
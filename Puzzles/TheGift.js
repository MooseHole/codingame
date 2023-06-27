var budgets = [];

function addAllBudgets() {
    let sum = 0;
    for (let i = 0; i < budgets.length; i++) {
        sum += budgets[i];
    }
    return sum;
}

const NumberOfBudgets = parseInt(readline());
const TotalCost = parseInt(readline());
var remainingCost = TotalCost;

for (let i = 0; i < NumberOfBudgets; i++) {
    budgets[i] = parseInt(readline());
}

var total = addAllBudgets();

if (total < remainingCost) {
    console.log('IMPOSSIBLE');
    exit();
}

var sortedBudgets = budgets.sort((a, b) => a - b);

var numberOfMaxedBudgets = 0;
for (let i = 0; i < NumberOfBudgets; i++) {
    var average = Math.floor(remainingCost / (NumberOfBudgets-i));
    if (sortedBudgets[i] < average) {
        console.log(sortedBudgets[i]);
        remainingCost -= sortedBudgets[i];
        numberOfMaxedBudgets++;
    }
    else {
        break;
    }
}

var remainingBudgetsToProcess = NumberOfBudgets-numberOfMaxedBudgets;
var average = Math.floor(remainingCost / remainingBudgetsToProcess);
var averagePlusOne = average + 1;
var remainderExists = remainingCost - (average * remainingBudgetsToProcess) != 0;
for (let i = 0; i < remainingBudgetsToProcess; i++) {
    console.log(average);
    remainingCost -= average;
    if (remainderExists && remainingCost === averagePlusOne * (remainingBudgetsToProcess - i - 1)) {
        average = averagePlusOne;
    }
}
